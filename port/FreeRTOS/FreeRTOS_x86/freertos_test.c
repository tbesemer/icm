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

static SemaphoreHandle_t semTask1;
static SemaphoreHandle_t semTask2;
static void freertosTestTask1( void *pvParameters );
static void freertosTestTask2( void *pvParameters );

int main()
{
    xTaskCreate( freertosTestTask1, (const char * )"testTask1", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1), NULL );
    xTaskCreate( freertosTestTask2, (const char * )"testTask2", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2), NULL );

    fprintf( stdout, "main(): Calling vTaskStartScheduler()\n" );
    vTaskStartScheduler();
    fprintf( stderr, "main(): vTaskStartScheduler() Returned\n" );

    exit( 0 );
}

static void freertosTestTask1( void *pvParameters )
{

    printf( "freertosTestTask1(): running, 0x%p\n", pvParameters );
    semTask1 = xSemaphoreCreateCounting( 10, 0 );

    for( ;; ) {
	printf( "freertosTestTask1(): Blocking onxSemaphoreTake()\n" );
	xSemaphoreTake( semTask1, 1000 );
	printf( "freertosTestTask1(): back from xSemaphoreTake() ===>\n" );
	xSemaphoreGive( semTask2 );
        taskYIELD();
    }
}

static void freertosTestTask2( void *pvParameters )
{

    printf( "freertosTestTask2(): running, 0x%p\n", pvParameters );
    semTask2 = xSemaphoreCreateCounting( 10, 0 );

    for( ;; ) {
	printf( "freertosTestTask2(): Blocking on xSemaphoreTake()\n" );
	xSemaphoreTake( semTask2, portMAX_DELAY );
	printf( "<=== freertosTestTask2(): back from xSemaphoreTake()\n" );
    }
}

