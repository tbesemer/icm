#include        <stdint.h>
#include        <stdio.h>
#include        <stdarg.h>
#include        <strings.h>
#include        <stdlib.h>
#include        <pthread.h>
#include        <semaphore.h>
#include        <icm_pub.h>
#include        <icm_private.h>
#include        <icm_osal.h>

#define ICM_OSAL_MAX_QUEUE_CNT 200

typedef struct {

    /*  Thread Control
     */
    pthread_t pthreadTask;

    /* Message Queue 
     */
    sem_t       sem;
    int         size;
    int         head;
    int         tail;
    ICM_MSG     *msgs[ ICM_OSAL_MAX_QUEUE_CNT ];

} ICM_OSAL_CTL;

/*  Locks;
 */
typedef struct {

    int       valid;
    sem_t     lock;

} ICM_OSAL_LOCK_CTL;

static   ICM_OSAL_LOCK_CTL   icmOsLocks[ ICM_SEM_MAX ];

int icmOsInit()
{
    bzero( (void *)&icmOsLocks, sizeof( icmOsLocks ) );
    return( 1 );
}

int icmOsStart()
{
    fprintf( stdout, "icmOsStart(): Doing Nothing()\n" );
    return( 1 );
}

int icmOsCreateLock( ICM_SEM_INDEXES index, int val )
{

    if( (index < 0) || (index >= ICM_SEM_MAX) ) {
        icmErrorLog( "icmOsCreateLock(): FAIL, index %d out of range\n", index );
	return( 0 );
    }

    if( sem_init( &icmOsLocks[ index ].lock, 0, (unsigned int)val ) ) {
        icmErrorLog( "icmOsCreateLock(): FAIL, sem_init() failed\n" );
	return( 0 );
    }

    icmOsLocks[ index ].valid = 1;
    return( 1 );
}

int icmOsLockAcquire( ICM_SEM_INDEXES index )
{
    if( (index < 0) || (index >= ICM_SEM_MAX) ) {
        icmErrorLog( "icmOsLockAcquire(): FAIL, index %d out of range\n", index );
	return( 0 );
    }

    if( !icmOsLocks[ index ].valid ) {
        icmErrorLog( "icmOsLockAcquire(): FAIL, index %d Not Valid\n", index );
	return( 0 );
    }

    if( sem_wait( &icmOsLocks[ index ].lock ) ) {
        icmErrorLog( "icmOsLockAcquire(): FAIL, sem_wait() failed\n" );
	return( 0 );
    }

    return( 1 );
}

int icmOsLockRelease( ICM_SEM_INDEXES index )
{
    if( (index < 0) || (index >= ICM_SEM_MAX) ) {
        icmErrorLog( "icmOsLockRelease(): FAIL, index %d out of range\n", index );
	return( 0 );
    }

    if( !icmOsLocks[ index ].valid ) {
        icmErrorLog( "icmOsLockRelease(): FAIL, index %d Not Valid\n", index );
	return( 0 );
    }

    if( sem_post( &icmOsLocks[ index ].lock ) ) {
        icmErrorLog( "icmOsLockRelease(): FAIL, sem_post() failed\n" );
	return( 0 );
    }

    return( 1 );
}


int icmOsTaskCreate( ICM_OS_TASK_CTL *taskCtlPtr, ICM_TASK icmTask, int pri )
{
ICM_OSAL_CTL *osCtlStruct;

    icmLog( ICM_LOG_V1, "icmOsTaskCreate(): Called\n" ); 

    osCtlStruct = (ICM_OSAL_CTL *)taskCtlPtr->osExtension;

    if( pthread_create( &osCtlStruct->pthreadTask, NULL, icmTask, (void *)(taskCtlPtr->trueIndex + 1) ) ) {
	icmErrorLog( "icmOsTaskCreate(): pthread_create() FAILED\n" );
	return( 0 );
    }

    return( 1 );
}

int icmOsTaskDelete( ICM_OS_TASK_CTL *taskCtlPtr )
{
    return( 1 );
}

int icmOsQueueCreate( ICM_OS_TASK_CTL *taskCtlPtr, int size )
{
ICM_OSAL_CTL *osCtlStruct;

    icmLog( ICM_LOG_V1, "icmOsQueueCreate(): Called\n" ); 

    if( size > ICM_OSAL_MAX_QUEUE_CNT ) {
        icmErrorLog( "icmOsQueueCreate(): FAIL, size of Queue %d exceeds system imposed limit\n" ); 
	return( 0 );
    }

    osCtlStruct = (ICM_OSAL_CTL *)malloc( sizeof( ICM_OSAL_CTL ) );
    if( !osCtlStruct ) {
	icmErrorLog( "icmOsQueueCreate(): malloc() FAILED\n" );
	return( 0 );
    }

    bzero( (void *)osCtlStruct, sizeof( ICM_OSAL_CTL ) );

    taskCtlPtr->osExtension = (void *)osCtlStruct;

    osCtlStruct->size = size;
    osCtlStruct->head = 0;
    osCtlStruct->tail = 0;

    if( sem_init( &osCtlStruct->sem, 0, 0 ) ) {
        icmErrorLog( "icmOsQueueCreate(): FAIL, sem_init() failed\n" );
	return( 0 );
    }

    return( 1 );
}

int icmOsQueueDelete( ICM_OS_TASK_CTL *taskCtlPtr )
{
    // sem_destory( &osCtlStruct->sem );
    return( 1 );
}

int icmOsPostEvent( ICM_OS_TASK_CTL *taskCtlPtr, ICM_MSG *icmMsg )
{
ICM_OSAL_CTL *osCtlStruct;
int          tmpHead;

    icmLog( ICM_LOG_V2, "icmOsPostEvent(): Posting mainEvent %d\n", icmMsg->mainEvent );

    osCtlStruct = (ICM_OSAL_CTL *)taskCtlPtr->osExtension;
    tmpHead = osCtlStruct->head;
    tmpHead++;
    if( tmpHead == osCtlStruct->size ) {
	tmpHead = 0;
    }

    if( tmpHead == osCtlStruct->tail ) {
        icmErrorLog( "icmOsPostEvent(): FAIL, Queue Full\n" );
	return( 0 );
    }

    icmLog( ICM_LOG_V2, "icmOsPostEvent(): icmMsg == %p\n",icmMsg );
    osCtlStruct->msgs[ osCtlStruct->head ] = icmMsg;
    osCtlStruct->head = tmpHead;

    if( sem_post( &osCtlStruct->sem ) ) {
        icmErrorLog( "icmOsPostEvent(): FAIL, sem_post() failed\n" );
	return( 0 );
    }

    return( 1 );
}

ICM_MSG *icmOsWaitEvent( ICM_OS_TASK_CTL *taskCtlPtr )
{
ICM_MSG *icmMsg;
ICM_OSAL_CTL *osCtlStruct;
int          tmpTail;

    icmLog( ICM_LOG_V2, "icmOsWaitEvent(): Waiting\n" );

    osCtlStruct = (ICM_OSAL_CTL *)taskCtlPtr->osExtension;

    if( sem_wait( &osCtlStruct->sem ) ) {
        icmErrorLog( "icmOsWaitEvent(): FAIL, sem_wait() failed\n" );
	return( (ICM_MSG *)NULL );
    }

    tmpTail = osCtlStruct->tail;
    tmpTail++;
    if( tmpTail == osCtlStruct->size ) {
	tmpTail = 0;
    }

    icmMsg = osCtlStruct->msgs[ osCtlStruct->tail ];
    osCtlStruct->tail = tmpTail;

    return( icmMsg );
}
