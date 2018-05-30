#ifndef _ICM_PRIVATE_H__
#define _ICM_PRIVATE_H__


typedef struct  {

/* 0x00 */    uint16_t   mainEvent;
/* 0x02 */    uint16_t   subEvent;
/* 0x04 */    uint16_t   srcId;
/* 0x06 */    uint16_t   dstId;
/* 0x08 */    uint16_t   msgSize;
/* 0x0A */     int16_t   refCnt;
/* 0x0C */    uint16_t   allocPool;

} ICM_MSG_HDR;

typedef struct {

    int size;      /* Size of Message Body        */
    int cnt;       /* Total Event Notices in Pool */

} ICM_FP_CONFIG;

/* Total number of Free Pools
 */
#define ICM_MAX_FREE_POOL 2
#define	ICM_MAX_FP_SIZE   512

typedef struct {

    int         sp;
    int         cnt;
    int         size;
    ICM_MSG_HDR *msgPoolArray[ ICM_MAX_FP_SIZE ]; 

} ICM_FP;

/*  Must be a mutiple of sizeof( uint32_t )
 */
#define	ICM_MAX_TASKS	64
#define	ICM_DISPATCH_LIST_SIZE ICM_MAX_TASKS

/*  One per ICM Task; it contains both theT Task ID, and the Queue Id.
 */
typedef struct {

    uint32_t     inUse;
    uint32_t     trueIndex; /* Index into taskCtl in ICM_WORKSPACE */
    uint32_t     qHandle;
    uint32_t     tHandle;
    void         *osExtension;  

} ICM_OS_TASK_CTL;

#define	ICM_MAX_DISPATCH_LIST	ICM_MAX_TASKS

/*  Primary Workspace.
 */
typedef struct {

    ICM_OS_TASK_CTL     taskCtl[ ICM_MAX_TASKS ];

    /*  Dynamically created Free Pool of Event Notices.  Refrerence
     *  ~/config/icm_config.c for configuration.
     */
    ICM_FP              icmFp[ ICM_MAX_FREE_POOL ];

    uint8_t		dispatchList[ ICM_MAX_MAIN_EVENT ][ ICM_DISPATCH_LIST_SIZE ];


    /* Debug Flag; greater values, more verbose.
     */
    int debug;

} ICM_WORKSPACE;

void icmDumpFreePoolConfig();
void icmDumpFreePoolStatus();
void icmInitFreePools( int *err );
ICM_MSG_HDR *icmGetFromPool( int size, int *err );
void icmFreeToPool( ICM_MSG_HDR *icmMsg, int *err );
void icmInitLocks( int *err );
void icmLockFp();
void icmUnlockFp();
void icmLockDispatch();
void icmUnlockDispatch();
void icmLockWorkspace();
void icmUnlockWorkspace();
void icmOsLockScheduler();
void icmOsUnlockScheduler();

/*  Semaphore Indexes, zero based, used by OSAL to store 
 *  semaphore objects in local structure.
 */
typedef enum {

    ICM_SEM_FP = 0,
    ICM_SEM_DISPATCH = 1,
    ICM_SEM_WKSPC = 2,
    ICM_SEM_MAX = 3,

} ICM_SEM_INDEXES;

extern ICM_WORKSPACE  *icmWkspc;

#endif
