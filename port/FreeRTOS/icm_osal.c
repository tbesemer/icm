#include        <stdint.h>
#include        <stdio.h>
#include        <stdarg.h>
#include        <strings.h>
#include        <stdlib.h>
#include	<time.h>
#include	<sys/time.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>

#include	<FreeRTOS.h>
#include	<task.h>
#include	<queue.h>
#include	<semphr.h>
#include	<croutine.h>

#include        <icm_pub.h>
#include        <icm_private.h>
#include        <icm_osal.h>

#define ICM_OSAL_MAX_QUEUE_CNT 200

typedef struct {

    /*  Thread Control
     */
    TaskHandle_t pxCreatedTask;

    /* Message Queue 
     */
    SemaphoreHandle_t  sem;
    int                size;
    int                head;
    int                tail;
    ICM_MSG            *msgs[ ICM_OSAL_MAX_QUEUE_CNT ];

} ICM_OSAL_CTL;

/*  Locks;
 */
typedef struct {

    int                valid;
    SemaphoreHandle_t  lock;

} ICM_OSAL_LOCK_CTL;

static   ICM_OSAL_LOCK_CTL   icmOsLocks[ ICM_SEM_MAX ];

int icmOsInit()
{
    bzero( (void *)&icmOsLocks, sizeof( icmOsLocks ) );
    return( 1 );
}

int icmOsStart()
{

    fprintf( stdout, "icmOsStart(): Calling vTaskStartScheduler()\n" );
    vTaskStartScheduler();
    fprintf( stderr, "icmOsStart(): vTaskStartScheduler() Returned\n" );
    return( 0 );
}

int icmOsCreateLock( ICM_SEM_INDEXES index, int val )
{

    if( (index < 0) || (index >= ICM_SEM_MAX) ) {
        icmErrorLog( "icmOsCreateLock(): FAIL, index %d out of range\n", index );
	return( 0 );
    }

    icmOsLocks[ index ].lock = xSemaphoreCreateCounting( 1, val );
    if( !icmOsLocks[ index ].lock ) {
        icmErrorLog( "icmOsCreateLock(): FAIL, xSemaphoreCreateCountingi() failed\n" );
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

    if( xSemaphoreTake( icmOsLocks[ index ].lock, portMAX_DELAY ) == pdFALSE ) {
        icmErrorLog( "icmOsLockAcquire(): FAIL, xSemaphoreTake() failed\n" );
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

    if( xSemaphoreGive( icmOsLocks[ index ].lock ) == pdFALSE ) {
        icmErrorLog( "icmOsLockRelease(): FAIL, xSemaphoreGive() failed\n" );
	return( 0 );
    }

    return( 1 );
}


int icmOsTaskCreate( ICM_OS_TASK_CTL *taskCtlPtr, ICM_TASK icmTask, int pri )
{
ICM_OSAL_CTL *osCtlStruct;

    icmLog( ICM_LOG_V1, "icmOsTaskCreate(): Called\n" ); 

    osCtlStruct = (ICM_OSAL_CTL *)taskCtlPtr->osExtension;

    /* FIXME - Error Checking */
    xTaskCreate( (pdTASK_CODE)icmTask, (const char *)"icmTask", configMINIMAL_STACK_SIZE, (void *)(unsigned long int)(taskCtlPtr->trueIndex + 1), (tskIDLE_PRIORITY + pri), &osCtlStruct->pxCreatedTask );

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

    osCtlStruct->sem = xSemaphoreCreateCounting( size, 0 );
    if( !osCtlStruct->sem ) {
        icmErrorLog( "icmOsQueueCreate(): FAIL, xSemaphoreCreateCounting() failed\n" );
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

    if( xSemaphoreGive( osCtlStruct->sem ) == pdFALSE ) {
        icmErrorLog( "icmOsPostEvent(): FAIL, xSemaphoreGive() failed\n" );
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

    if( xSemaphoreTake( osCtlStruct->sem, portMAX_DELAY ) == pdFALSE ) {
        icmErrorLog( "icmOsWaitEvent(): FAIL, xSemaphoreTake() failed\n" );
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

void icmOsLockScheduler()
{
    vTaskSuspendAll();
}

void icmOsUnlockScheduler()
{

    xTaskResumeAll();
}

#ifdef __GCC_POSIX__
void vApplicationIdleHook( void )
{
	struct timespec xTimeToSleep, xTimeSlept;
		/* Makes the process more agreeable when using the Posix simulator. */
		xTimeToSleep.tv_sec = 1;
		xTimeToSleep.tv_nsec = 0;
		nanosleep( &xTimeToSleep, &xTimeSlept );
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
        taskENTER_CRITICAL();
        {
        printf("[ASSERT] %s:%lu\n", pcFileName, ulLine);
        fflush(stdout);
        }
        taskEXIT_CRITICAL();
        exit(-1);
}

void vApplicationTickHook( void )
{
        /* This function will be called by each tick interrupt if
        configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
        added here, but the tick hook is called from an interrupt context, so
        code must not attempt to block, and only the interrupt safe FreeRTOS API
        functions can be used (those that end in FromISR()). */

}

void vApplicationMallocFailedHook( void )
{
        /* vApplicationMallocFailedHook() will only be called if
        configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
        function that will get called if a call to pvPortMalloc() fails.
        pvPortMalloc() is called internally by the kernel whenever a task, queue,
        timer or semaphore is created.  It is also called by various parts of the
        demo application.  If heap_1.c or heap_2.c are used, then the size of the
        heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
        FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
        to query the size of free heap space that remains (although it does not
        provide information on how the remaining heap might be fragmented). */
        vAssertCalled( __LINE__, __FILE__ );
}
#endif
