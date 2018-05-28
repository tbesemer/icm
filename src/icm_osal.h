#ifndef __ICM_OSAL_H__
#define __ICM_OSAL_H__

int icmOsInit();
int icmOsStart();
int icmOsLockAcquire( ICM_SEM_INDEXES index );
int icmOsLockRelease( ICM_SEM_INDEXES index );
int icmOsCreateLock( ICM_SEM_INDEXES index, int val );
int icmOsTaskCreate( ICM_OS_TASK_CTL *taskCtlPtr, ICM_TASK icmTask, int pri );
int icmOsTaskDelete( ICM_OS_TASK_CTL *taskCtlPtr );
int icmOsQueueCreate( ICM_OS_TASK_CTL *taskCtlPtr, int size );
int icmOsQueueDelete( ICM_OS_TASK_CTL *taskCtlPtr );
int icmOsPostEvent( ICM_OS_TASK_CTL *taskCtlPtr, ICM_MSG *icmMsg );
ICM_MSG *icmOsWaitEvent( ICM_OS_TASK_CTL *taskCtlPtr );


#endif
