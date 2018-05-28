#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <croutine.h>

void vApplicationIdleHook( void );
void vAssertCalled( unsigned long ulLine, const char * const pcFileName );
void vApplicationTickHook( void );
void vApplicationMallocFailedHook( void );


void vApplicationIdleHook( void )
{
#ifdef __GCC_POSIX__
	struct timespec xTimeToSleep, xTimeSlept;
		/* Makes the process more agreeable when using the Posix simulator. */
		xTimeToSleep.tv_sec = 1;
		xTimeToSleep.tv_nsec = 0;
		nanosleep( &xTimeToSleep, &xTimeSlept );
#endif
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
