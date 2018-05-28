#include	<stdint.h>
#include	<stdio.h>
#include	<stdarg.h>
#include	<strings.h>
#include	<icm_pub.h>
#include	<icm_private.h>
#include	<icm_osal.h>


static ICM_WORKSPACE  icmWkspcAlloc;
       ICM_WORKSPACE  *icmWkspc = &icmWkspcAlloc;

void icmInit( int *err, int debugLevel )
{
    if( !icmOsInit() ) {
	icmErrorLog( "icmInit(): icmOsInit() failed\n" );
	*err = ICM_ERR_OS_INIT;
	return;
    }

    bzero( (void *)icmWkspc, sizeof( ICM_WORKSPACE ) );
    icmWkspc->debug = debugLevel;
    icmLog( ICM_LOG_ALWAYS, "icmInit(): debug = %d\n", icmWkspc->debug );

    icmInitFreePools( err );
    if( *err != ICM_NO_ERR ) {
	icmErrorLog( "icmInit(): icmInitFreePools() failed, err = %d\n", *err );
	return;
    }

    icmInitLocks( err );
    if( *err != ICM_NO_ERR ) {
	icmErrorLog( "icmInit(): icmInitLocks() failed, err = %d\n", *err );
	return;
    }

}

ICM_MSG *icmAllocEvent( int size,  int *err )
{
ICM_MSG *icmMsg;

    icmMsg = (ICM_MSG *)icmGetFromPool( size, err );
    if( *err != ICM_NO_ERR ) {
	icmErrorLog( "icmAllocEvent(): icmGetFromPool() failed, err = %d\n", *err );
    }

    return( icmMsg );
}

void icmFreeEvent( ICM_MSG *icmMsg,  int *err )
{

    icmFreeToPool( (ICM_MSG_HDR *)icmMsg, err );
    if( *err != ICM_NO_ERR ) {
	icmErrorLog( "icmFreeEvent(): icmFreeToPool() failed, err = %d\n", *err );
    }

}

static int icmValidateHandlerID( ICM_HANDLER hId )
{
int taskCtlIndex;

    /* We are always (hId + 1) out in the wild, to ensure that
     * zero is not used.  Adjust to internal.
     */
    taskCtlIndex = (hId - 1);

    if( (taskCtlIndex < 0) || (taskCtlIndex >= ICM_MAX_TASKS) ) {
	icmErrorLog( "icmValidateHandlerID(): FAIL, Invalid hId %d\n", hId );
	return( 0 );
    }

    if( !icmWkspc->taskCtl[ taskCtlIndex ].inUse ) {
	icmErrorLog( "icmValidateHandlerID(): FAIL, hId %d not inUse\n", hId );
	return( 0 );
    }

    return( 1 );
}

ICM_HANDLER icmOpenHandler( int size, int pri, ICM_TASK icmTask, int *err )
{
int taskCtlIndex;
ICM_OS_TASK_CTL  *taskCtlPtr;

    /* In case we fail.
     */
    taskCtlPtr = (ICM_OS_TASK_CTL *)NULL;
    *err = ICM_ERR_TASK_CREATE;

    /* Allocate an empty Task Control Block.
     */
    icmLockWorkspace();
    for( taskCtlIndex = 0; taskCtlIndex < ICM_MAX_TASKS; taskCtlIndex++ ) {
	if( !icmWkspc->taskCtl[ taskCtlIndex ].inUse ) {
	    taskCtlPtr = &icmWkspc->taskCtl[ taskCtlIndex ];
	    icmLog( ICM_LOG_V2, "icmOpenHandler(): taskCtlPtr = 0x%016lX, taskCtlIndex = %d\n",
		taskCtlPtr, taskCtlIndex );
	    break;
	}
    }

    if( !taskCtlPtr ) {
	icmErrorLog( "icmOpenHandler(): FAIL, Task Control Structures Depleted\n" );
        icmUnlockWorkspace();
	return( 0 );
    }

    taskCtlPtr->trueIndex = taskCtlIndex;

    /*  Create Task/Queue for ICM Instance.  If they do not fail, mark
     *  Control Block as in use.  Queue created first so once task
     *  starts up, it can block on it.
     */
    if( icmOsQueueCreate( taskCtlPtr, size ) ) {
        if( icmOsTaskCreate( taskCtlPtr, icmTask, pri ) ) {
	    taskCtlPtr->inUse = 1;
            *err = ICM_NO_ERR;
	} else {
	    icmErrorLog( "icmOpenHandler(): FAIL, icmOsTaskCreate() failed\n" );
	    icmOsQueueDelete( taskCtlPtr );
	}
    } else {
	icmErrorLog( "icmOpenHandler(): FAIL, icmOsQueueCreate() failed\n" );
    }

    icmUnlockWorkspace();

    /*  Task Control Index is always (index + 1) to help ensure
     *  NULL values are not used.
     */
    return( (taskCtlIndex + 1) ); 
}

void icmCloseHandler( ICM_HANDLER hId, int *err )
{
    icmLog( ICM_LOG_ALWAYS, "icmCloseHandler(): Unsupported\n" );
    *err = ICM_NO_ERR;
}

void icmDispatch( ICM_MSG *icmMsg, int *err )
{
int mainEvent, dispatchIndex;
uint8_t *dispatchList;
ICM_OS_TASK_CTL  *taskCtlPtr;

    *err = ICM_ERR_DISPATCH;

    mainEvent = icmMsg->mainEvent;
    if( (mainEvent < 0) || (mainEvent >= ICM_MAX_MAIN_EVENT) ) {
	icmErrorLog( "icmDispatch(): FAIL, mainEvent %d invalid\n", mainEvent );
	*err = ICM_ERR_MAIN_EVENT;
    }

    icmLockDispatch();
    dispatchList = icmWkspc->dispatchList[ mainEvent ];
    for( dispatchIndex = 0; dispatchIndex < ICM_DISPATCH_LIST_SIZE; dispatchIndex++ ) {
	if( dispatchList[ dispatchIndex ] ) {

	    taskCtlPtr = &icmWkspc->taskCtl[ dispatchIndex ];

	    icmLog( ICM_LOG_V2, "icmDispatch(): taskCtlPtr = 0x%016lX, dispatchIndex = %d\n",
			(long unsigned int)taskCtlPtr, dispatchIndex );
	    icmLog( ICM_LOG_V2, "icmDispatch(): icmMsg = %d\n", icmMsg );

	    icmMsg->refCnt++;
	    icmOsPostEvent( taskCtlPtr, icmMsg );
	}
    }
    icmUnlockDispatch();

}

ICM_MSG *icmMsgWait( ICM_HANDLER hId, int *err )
{
ICM_MSG         *icmMsg;
ICM_OS_TASK_CTL *taskCtlPtr;

    if( !icmValidateHandlerID( hId ) ) {
	icmErrorLog( "icmMsgWait(): FAILED, hId %d invalid\n", hId );
	*err = ICM_ERR_TASK_HANDLE;
	return( (ICM_MSG *)NULL );
    }

    taskCtlPtr = &icmWkspc->taskCtl[ (hId - 1) ];
    icmMsg = icmOsWaitEvent( taskCtlPtr );
    if( !icmMsg ) {
	*err = ICM_ERR_WAIT;
	return( (ICM_MSG *)NULL );
    }

    return( icmMsg );
}

void icmQueueAssociate( ICM_MAIN_EVENT mainEvent, ICM_HANDLER hId, int *err )
{
    if( !icmValidateHandlerID( hId ) ) {
	icmErrorLog( "icmQueueAssociate(): FAIL, hId %d invalid\n", hId );
	*err = ICM_ERR_TASK_HANDLE;
	return;
    }

    if( (mainEvent < 0) || (mainEvent >= ICM_MAX_MAIN_EVENT) ) {
	icmErrorLog( "icmQueueAssociate(): FAIL, mainEvent %d invalid\n", mainEvent );
	*err = ICM_ERR_MAIN_EVENT;
    }

    /* We are always (hId + 1) out in the wild, to ensure that
     * zero is not used.  Adjust to internal.
     *
     * Check to see if we are associated; it's harmless to
     * associate more than once, but to help the code out
     * in the wild. we flag it as an error.
     */
    if( icmWkspc->dispatchList[ mainEvent ][ (hId - 1) ] ) {
	icmErrorLog( "icmQueueAssociate(): FAIL, Already Associated\n", mainEvent );
	*err = ICM_ERR_ASSOCIATION;
    } else {
        icmWkspc->dispatchList[ mainEvent ][ (hId - 1) ] = 1;
    }

}

