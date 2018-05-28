#include        <stdint.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <unistd.h>
#include        <icm_pub.h>
#include        <icm_private.h>

/* Size of Event Body (payload).
 */
#define	ICM_TEST_EVENT_SIZE	200

typedef enum {

    TEST_ICM_MAIN_10 = 10,
    TEST_ICM_MAIN_20 = 20,

} TEST_ICM_MAIN_EVENTS;


static void *myTestTask1( void *v1 );

#define	TEST_TASK1_QUEUE_SIZE	20
#define	TEST_TASK1_PRIORITY	10

static int myTestTask1Init()
{
int         err;
ICM_HANDLER taskHandle;

    /* Create the Handler (pthread under Linux, FreeRTOS Task under FreeRTOS).
     */
    taskHandle = icmOpenHandler( TEST_TASK1_QUEUE_SIZE, TEST_TASK1_PRIORITY, myTestTask1, &err );
    if( err ) {
        icmLog( ICM_LOG_ALWAYS, "myTestTask1Init(): icmOpenHandler() FAILED, err = %d\n", err );
    } else {
        icmLog( ICM_LOG_ALWAYS, "myTestTask1Init(): taskHandle = %d, err = %d\n", taskHandle, err );
    }

    /*  Subscribe the handler to TEST_ICM_MAIN_20
     */
    icmQueueAssociate( TEST_ICM_MAIN_20, taskHandle, &err );
    if( err ) {
        icmLog( ICM_LOG_ALWAYS, "myTestTask1Init(): icmOpenHandler() FAILED, err = %d\n", err );
    } else {
        icmLog( ICM_LOG_ALWAYS, "myTestTask1Init(): icmQueueAssociate() #1 GOOD, returned %d\n", err );
    }

    return( 0 );
}

static void *myTestTask1( void *v1 )
{
ICM_MSG *icmMsg;
int err;
ICM_HANDLER hId = (ICM_HANDLER)(long)v1;

    icmLog( ICM_LOG_ALWAYS, "mytTestTask1(): running, v1 = %d\n", (int)(long)v1 );

    for( ;; ) {
        icmMsg = icmMsgWait( hId, &err );
        if( !icmMsg ) {
            icmLog( ICM_LOG_ALWAYS, "myTestTask1(): Back from icmMsgWait(), icmMsg NULL, err = %d\n", err );
        } else {
            icmLog( ICM_LOG_ALWAYS, "myTestTask1(): Back from icmMsgWait(), icmMsg->mainEvent = %d\n", icmMsg->mainEvent );
        }
    }
    
    return( NULL );
}

static void *myTestTask2( void *v1 );

#define	TEST_TASK2_QUEUE_SIZE	30
#define	TEST_TASK2_PRIORITY	11

static int myTestTask2Init()
{
int         err;
ICM_HANDLER taskHandle;

    /* Create the Handler (pthread under Linux, FreeRTOS Task under FreeRTOS).
     */
    taskHandle = icmOpenHandler( TEST_TASK2_QUEUE_SIZE, TEST_TASK2_PRIORITY, myTestTask2, &err );
    if( err ) {
        icmLog( ICM_LOG_ALWAYS, "myTestTask2Init(): icmOpenHandler() FAILED, err = %d\n", err );
    } else {
        icmLog( ICM_LOG_ALWAYS, "myTestTask2Init(): taskHandle = %d, err = %d\n", taskHandle, err );
    }

    /*  Subscribe the handler to TEST_ICM_MAIN_20, and TEST_ICM_MAIN_10
     */
    icmQueueAssociate( TEST_ICM_MAIN_20, taskHandle, &err );
    if( err ) {
        icmLog( ICM_LOG_ALWAYS, "myTestTask2Init(): icmOpenHandler() #1 FAILED, err = %d\n", err );
    } else {
        icmLog( ICM_LOG_ALWAYS, "myTestTask2Init(): icmQueueAssociate() #1 GOOD, returned %d\n", err );
    }

    icmQueueAssociate( TEST_ICM_MAIN_10, taskHandle, &err );
    if( err ) {
        icmLog( ICM_LOG_ALWAYS, "myTestTask2Init(): icmOpenHandler() #2 FAILED, err = %d\n", err );
    } else {
        icmLog( ICM_LOG_ALWAYS, "myTestTask2Init(): icmQueueAssociate() #2 GOOD, returned %d\n", err );
    }

    return( 0 );
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
int     err, cnt;
ICM_MSG *icmMsg;

    /*  Initalize the ICM Sub-System.
     */
    icmLog( ICM_LOG_ALWAYS, "main(): Calling icm_init()\n" );
    icmInit( &err, ICM_LOG_V3 );
    if( err ) {
        icmLog( ICM_LOG_ALWAYS, "main(): icmInit() FAILED, err = %d\n", err );
	exit( 1 );
    }

    /*  Create the Test Tasks.
     */
    if( myTestTask1Init() ) {
        icmLog( ICM_LOG_ALWAYS, "main(): myTestTask1Init() FAILED, Exiting\n" );
	exit( 1 );
    }

    if( myTestTask2Init() ) {
        icmLog( ICM_LOG_ALWAYS, "main(): myTestTask2Init() FAILED, Exiting\n" );
	exit( 1 );
    }

    sleep( 1 );
    icmMsg = icmAllocEvent( ICM_TEST_EVENT_SIZE, &err );
    icmMsg->mainEvent = 20;
    icmDispatch( icmMsg, &err );

    /*  Under POSIX Linux, using pthread()'s, this call returns.  Thus, for test
     *  case, we continue to run as parent main().  Under FreeRTOS, this call
     *  never returns, normal tasking starts up.
     */
    icmLog( ICM_LOG_ALWAYS, "main(): calling icmOsStart(), Returns under POSIX Linux\n" );
    icmOsStart();
 
    /*  Simply loop and send events, with cnt in subEvent, on based on even/odd,
     *  alternating between mainEvent of TEST_ICM_MAIN_10 and TEST_ICM_MAIN_20.
     */
    cnt = 0;
    for( ;; ) {
        icmMsg = icmAllocEvent( ICM_TEST_EVENT_SIZE, &err );
	if( err ) {
    	    icmLog( ICM_LOG_ALWAYS, "main(): icmAllocEvent() FAILED, err = %d\n", err );
	    exit( 1 );
	} else {
	    if( (cnt & 0x01) ) {
                icmMsg->mainEvent = TEST_ICM_MAIN_20;
	    } else {
                icmMsg->mainEvent = TEST_ICM_MAIN_10;
	    }

	    icmMsg->subEvent = cnt;

            icmDispatch( icmMsg, &err );
	    if( err ) {
    	        icmLog( ICM_LOG_ALWAYS, "main(): icmDistpach() FAILED, err = %d\n", err );
		exit( 1 );
	    }
	}

	icmLog( ICM_LOG_ALWAYS, "main(): Sleeping for 2 seconds\n" );
	sleep( 2 );
    }
}
