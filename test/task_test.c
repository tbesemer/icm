#include        <stdint.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <unistd.h>
#include        <icm_pub.h>
#include        <icm_private.h>

static void *myTestTask1( void *v1 )
{
ICM_MSG *icmMsg;
int err;
ICM_HANDLER hId = (ICM_HANDLER)(long)v1;

    icmLog( ICM_LOG_ALWAYS, "mytTestTask1(): running, v1 = %d\n", (int)(long)v1 );

    for( ;; ) {
        icmMsg = icmMsgWait( hId, &err );
        if( !icmMsg ) {
            icmLog( ICM_LOG_ALWAYS, "myTestTask1(): Back from icmMsgWait(), icmMsg NULL\n" );
        } else {
            icmLog( ICM_LOG_ALWAYS, "myTestTask1(): Back from icmMsgWait(), icmMsg->mainEvent = %d\n", icmMsg->mainEvent );
        }
    }
    
    return( NULL );
}

static void *myTestTask2( void *v1 )
{
ICM_MSG *icmMsg;
int err;
ICM_HANDLER hId = (ICM_HANDLER)(long)v1;

    icmLog( ICM_LOG_ALWAYS, "mytTestTask2(): running, v1 = %d\n", (int)(long)v1 );

    for( ;; ) {
        icmMsg = icmMsgWait( hId, &err );
        if( !icmMsg ) {
            icmLog( ICM_LOG_ALWAYS, "myTestTask2(): Back from icmMsgWait(), icmMsg NULL\n" );
        } else {
            icmLog( ICM_LOG_ALWAYS, "myTestTask2(): Back from icmMsgWait(), icmMsg->mainEvent = %d\n", icmMsg->mainEvent );
        }
    }

    return( NULL );
}

main()
{
int err;
ICM_HANDLER taskHandle[ 2 ];
ICM_MSG *icmMsg[ 2 ];

    icmLog( ICM_LOG_ALWAYS, "task_test(): Calling icm_init()\n" );
    icmInit( &err, ICM_LOG_V3 );
    if( err ) {
        icmLog( ICM_LOG_ALWAYS, "task_test(): icm_init() FAILED, err = %d\n", err );
	exit( 1 );
    }


    taskHandle[ 0 ] = icmOpenHandler( 20, 10, myTestTask1, &err );
    icmLog( ICM_LOG_ALWAYS, "task_test(): #1 taskHandle = %d, err = %d\n", taskHandle[ 0 ], err );

    taskHandle[ 1 ] = icmOpenHandler( 20, 10, myTestTask2, &err );
    icmLog( ICM_LOG_ALWAYS, "task_test(): #2 taskHandle = %d, err = %d\n", taskHandle[ 1 ], err );
   
    icmQueueAssociate( 20, taskHandle[ 0 ], &err );
    icmLog( ICM_LOG_ALWAYS, "task_test(): icmQueueAssociate() #1 returned %d\n", err );

    icmQueueAssociate( 25, taskHandle[ 1 ], &err );
    icmLog( ICM_LOG_ALWAYS, "task_test(): icmQueueAssociate() #2 returned %d\n", err );
    icmQueueAssociate( 20, taskHandle[ 1 ], &err );
    icmLog( ICM_LOG_ALWAYS, "task_test(): icmQueueAssociate() #3 returned %d\n", err );

    sleep( 1 );
    icmMsg[ 0 ] = icmAllocEvent( 200, &err );
    icmMsg[ 0 ]->mainEvent = 20;
    icmDispatch( icmMsg[ 0 ], &err );

    icmMsg[ 1 ] = icmAllocEvent( 200, &err );
    icmMsg[ 1 ]->mainEvent = 25;
    icmDispatch( icmMsg[ 1 ], &err );

#ifdef	ORIGINAL
    printf( "test_task(): sleeping 10 seconds\n" );
    sleep( 10 );
#else
    printf( "test_task(): calling icmOsStart()\n" );
    icmOsStart();
#endif

}
