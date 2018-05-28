#ifndef	__ICM_PUB_H__
#define	__ICM_PUB_H__

#define ICM_MSG_HDR_MSG_SIZE 2

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

typedef int      ICM_HANDLER;
typedef uint16_t ICM_MAIN_EVENT;

#define ICM_MAX_MAIN_EVENT 256
#define ICM_INVALID_MAIN_EVENT  0xFFFF

typedef void *(*ICM_TASK)(void *v1);

void icmInit( int *err, int debugLevel );
ICM_MSG *icmAllocEvent( int size,  int *err );
void icmFreeEvent( ICM_MSG *icmMsg,  int *err );
ICM_HANDLER icmOpenHandler( int size, int pri, ICM_TASK icmTask, int *err );
void icmCloseHandler( ICM_HANDLER hId, int *err );
void icmStartHandler( ICM_HANDLER hId, int *err );
void icmDispatch( ICM_MSG *icmMsg, int *err );
ICM_MSG *icmMsgWait( ICM_HANDLER hId, int *err );
void icmQueueAssociate( ICM_MAIN_EVENT mainEvent, ICM_HANDLER hId, int *err );
void icmSetDebugLevel( int level );
int icmGetDebugLevel();
void icmLog( int logLevel, const char *msg, ... );
void icmErrorLog( const char *msg, ... );

typedef enum {

    ICM_LOG_ALWAYS = 0,
    ICM_LOG_V1 = 1,
    ICM_LOG_V2 = 2,
    ICM_LOG_V3 = 3

} ICM_LOG_LEVEL;

typedef enum {

    ICM_NO_ERR = 0,
    ICM_ERR_ALLOC = -1,
    ICM_ERR_FREE = -2,
    ICM_ERR_REF_CNT = -3,
    ICM_ERR_ALLOC_SIZE = -4,
    ICM_ERR_CONFIG = -5,
    ICM_ERR_TASK_CREATE = -6,
    ICM_ERR_TASK_HANDLE = -7,
    ICM_ERR_MAIN_EVENT = -8,
    ICM_ERR_ASSOCIATION = -9,
    ICM_ERR_DISPATCH = -10,
    ICM_ERR_WAIT = -11,
    ICM_ERR_OS_INIT = -12,
    ICM_ERR_LOCKS = -13,

} ICM_ERR;

#endif

