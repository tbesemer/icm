# Intertask Communications Manager (ICM)

The ICM is a target/OS independent OS Abstraction Layer designed to promote consistency
in Embedded System Task/Thread models, and provide a uniform messaging scheme to
communicate between tasks in the system.  It currently works in the following configurations:
1. POSIX Model, under Linux, with pthreads() as the Thread Model.
2. FreeRTOS, under Linux, using a FreeRTOS Simulator, using FreeRTOS tasks.
3. FreeRTOS, under ARM, using the Xilinx release of FreeRTOS, and the associated XSDK tools.

## Concept Foundation

The foundation of the ICM is that all Tasks/Threads in the system wait for a consistent message
typed called and Event Notice:

```
typedef struct  {

/* 0x00 */    uint16_t   mainEvent;
/* 0x02 */    uint16_t   subEvent;
/* 0x04 */    uint16_t   srcId;
/* 0x06 */    uint16_t   dstId;
/* 0x08 */    uint16_t   msgSize;
/* 0x0A */     int16_t   refCnt;
/* 0x0C */    uint16_t   allocPool;
/* 0x10 */    uint16_t   msgBody[ ICM_MSG_HDR_MSG_SIZE ];

} ICM_MSG;

```
Pointers to a message of type `ICM_MSG`, and the system is configured to support a Free Pool of
Event Notices with variable size payloads contained in `msgBody`.

Processing tasks subscribe to Event Notices baed on `mainEvent`.  Multiple tasks can sign up for
a variety of `mainEvent`'s.  Messages are handled in two ways:

### Message Generation

All systems have "events" which result in requests for processing; changes in state, user
interface actions, timers expiring, etc..  Multiple tasks in the system may need to be
alerted to these requests, and subscribe to Events.  Generation of an Event Notice typically
is done as follows:

```
/* Size of Event Body (payload) in demo.
 */
#define ICM_TEST_EVENT_SIZE     200

ICM_MSG     *icmMsg;

    icmMsg = icmAllocEvent( ICM_TEST_EVENT_SIZE, &err );
    icmMsg->mainEvent = TEST_ICM_MAIN_20;
    icmDispatch( icmMsg, &err );
    icmFreeEvent( icmMsg, &err );

```
Normal use would often carry a payload of a known structure type in `msgBody`.

### Message Processing

All tasks in the system would follow a template.  This template, simplified, is like this:

```
static int myTestTask1Init()
{
int         err;
ICM_HANDLER taskHandle;

    /* Create the Handler (pthread under Linux, FreeRTOS Task under FreeRTOS).
     */
    taskHandle = icmOpenHandler( TEST_TASK1_QUEUE_SIZE, TEST_TASK1_PRIORITY, myTestTask1, &err );

    /*  Subscribe the handler to TEST_ICM_MAIN_20
     */
    icmQueueAssociate( TEST_ICM_MAIN_20, taskHandle, &err );
}

static void *myTestTask1( void *v1 )
{
int         err;
ICM_MSG     *icmMsg;
ICM_HANDLER hId = (ICM_HANDLER)(long)v1;

    /* Loop forever, waiting for Event Notices.
     */
    for( ;; ) {
        icmMsg = icmMsgWait( hId, &err );

        /* Process based on mainEvent.
          */
        switch( icmMsg->mainEvent ) {
            case TEST_ICM_MAIN_20:
                icmLog( ICM_LOG_ALWAYS, "myTestTask1(): Handling TEST_ICM_MAIN_20\n" );
                break;
            default:
                icmLog( ICM_LOG_ALWAYS, "myTestTask1(): Unknown mainEvent %d\n", icmMsg->mainEvent );
                break;
        }

        /*  Free the Event Notice.  After all subscribers have received and completed
         *  processing the Event Notice, it is released to the Free Pool.
         */
        icmFreeEvent( icmMsg, &err );
    }
}

```
