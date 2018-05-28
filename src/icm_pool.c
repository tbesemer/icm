#include        <stdint.h>
#include        <stdlib.h>
#include        <stdio.h>
#include        <icm_pub.h>
#include        <icm_private.h>
#include        <icm_osal.h>

extern ICM_FP_CONFIG fpConfig[];

void icmDumpFreePoolConfig()
{
int fp;

    icmLog( ICM_LOG_V1, "===\n" );
    icmLog( ICM_LOG_V1, "icmDumpFreePoolConfig(): ICM_MAX_FREE_POOL = %d\n", ICM_MAX_FREE_POOL );
    icmLog( ICM_LOG_V1, "fp     size         count \n" );
    for( fp = 0; fp < ICM_MAX_FREE_POOL; fp++ ) {
        icmLog( ICM_LOG_V1, "%04d   %08d     %08d\n", fp, fpConfig[ fp ].size, fpConfig[ fp ].cnt  );
    }
    icmLog( ICM_LOG_V1, "===\n" );
}

void icmDumpFreePoolStatus()
{
int fp;

    icmLog( ICM_LOG_V1, "===\n" );
    icmLog( ICM_LOG_V1, "icmDumpFreePoolStatus(): ICM_MAX_FREE_POOL = %d\n", ICM_MAX_FREE_POOL );
    icmLog( ICM_LOG_V1, "fp     size         cnt         sp \n" );
    for( fp = 0; fp < ICM_MAX_FREE_POOL; fp++ ) {
       icmLog( ICM_LOG_V1, "%04d   %08d     %08d    %08d\n", fp,
		icmWkspc->icmFp[ fp ].size, icmWkspc->icmFp[ fp ].cnt, icmWkspc->icmFp[ fp ].sp );
    }
    icmLog( ICM_LOG_V1, "===\n" );
}

static int icmCheckFreePoolConfig()
{
int fp, retCode;

    retCode = 0;

    for( fp = 0; fp < ICM_MAX_FREE_POOL; fp++ ) {
        if( fpConfig[ fp ].cnt > ICM_MAX_FP_SIZE ) {
	    icmErrorLog( "icmCheckFreePoolConfig(): fp %d count %d exceeds system imposed limit\n",
			fp, fpConfig[ fp ].cnt );
	    retCode += -1;
	}
    }

    return( retCode );
}

void icmInitFreePools( int *err )
{
int fp, eventIndex;
int sizeSpec, numFpEntry, totalMemSize;
char *allocPtr;
ICM_MSG_HDR *icmMsg;

    icmDumpFreePoolConfig();

    if( icmCheckFreePoolConfig() < 0 ) {
	icmErrorLog( "icmInitFreePools(): icmCheckFreePoolConfig() Failed, FATAL\n" );
	*err = ICM_ERR_ALLOC_SIZE;
	return;
    }

    for( fp = 0; fp < ICM_MAX_FREE_POOL; fp++ ) {
	numFpEntry = fpConfig[ fp ].cnt;
	sizeSpec = fpConfig[ fp ].size;
	totalMemSize = ((sizeof(ICM_MSG_HDR) + sizeSpec) * numFpEntry);

	allocPtr = malloc( totalMemSize );
	if( !allocPtr ) {
	    *err = ICM_ERR_ALLOC;
	    icmErrorLog( "icmInitFreePools(): malloc() failed, fp %d\n", fp );
	    return;
	} else {
	    icmLog( ICM_LOG_V3, "icmInitFreePools(): Allocated %d for fp %d\n", totalMemSize, fp );
	}

	for( eventIndex = 0; eventIndex < numFpEntry; eventIndex++ ) {
	    icmMsg = (ICM_MSG_HDR *)allocPtr;
	    icmMsg->allocPool = sizeSpec;
	    icmWkspc->icmFp[ fp ].msgPoolArray[ eventIndex ] = icmMsg;
	    allocPtr = (char *)(allocPtr + sizeof(ICM_MSG_HDR) + sizeSpec);

	}

	icmWkspc->icmFp[ fp ].sp = numFpEntry;
	icmWkspc->icmFp[ fp ].cnt = numFpEntry;
	icmWkspc->icmFp[ fp ].size = sizeSpec;

    }

    *err = ICM_NO_ERR;
    icmDumpFreePoolStatus();
    return;

}

ICM_MSG_HDR *icmGetFromPool( int size, int *err )
{
int fp;
ICM_MSG_HDR *icmMsg;

    icmMsg = NULL;
    *err = ICM_ERR_ALLOC;

    icmLog( ICM_LOG_V3, "icmGetFromPool(): size = %d\n", size );

    icmLockFp();
    for( fp = 0; fp < ICM_MAX_FREE_POOL; fp++ ) {
	if( size <= icmWkspc->icmFp[ fp ].size ) {
	    if( icmWkspc->icmFp[ fp ].sp  ) {

		/*  Stack Pointer is always (top + 1), so we pre-decrement; 
		 *  with this, if it's 0 when this is called, it's empty.
		 */
		icmMsg = icmWkspc->icmFp[ fp ].msgPoolArray[ --icmWkspc->icmFp[ fp ].sp ];

		/* Reference Count == 1; requester of message always frees after
                 * dispatch.  Thus, if nobody requested, it's free'd to pool.
                 */
		icmMsg->refCnt = 1;

		*err = ICM_NO_ERR;
		icmLog( ICM_LOG_V3, "icmGetFromPool(): Allocated from fp %d, size %d\n",
			fp, icmWkspc->icmFp[ fp ].size );
		break;
	    }
	}
    }
    icmUnlockFp();

    if( *err == ICM_ERR_ALLOC ) {
	icmErrorLog( "icmGetFromPool(): FAILED on size %d\n", size );
    }

    icmLog( ICM_LOG_V3, "icmGetFromPool(): Allocated 0x%016lX\n", (long unsigned int)icmMsg );
    return( icmMsg );
}

void icmFreeToPool( ICM_MSG_HDR *icmMsg, int *err )
{
int fp;

    *err = ICM_ERR_FREE;

    icmLog( ICM_LOG_V3, "icmFreeToPool(): icmMsg->allocPool = %d\n", icmMsg->allocPool );

    icmLockDispatch();

    --icmMsg->refCnt;
    if( icmMsg->refCnt > 0 ) {
	*err = ICM_NO_ERR;
	icmUnlockDispatch();
	return;
    }

    if( icmMsg->refCnt < 0 ) {
	*err = ICM_ERR_REF_CNT;
	icmErrorLog( "icmFreeToPool(): Negative Reference Count, icmMsg->allocPool = %d\n",
		icmMsg->allocPool );
	icmErrorLog( "                 icmMsg == 0x%016lX, mainEvent = %d (0x%04X)\n", 
				(long unsigned int)icmMsg, icmMsg->mainEvent, icmMsg->mainEvent );
	icmUnlockDispatch();
	return;
    }

    icmUnlockDispatch();

    *err = ICM_ERR_ALLOC_SIZE;

    icmLockFp();
    for( fp = 0; fp < ICM_MAX_FREE_POOL; fp++ ) {
	if( icmMsg->allocPool == icmWkspc->icmFp[ fp ].size ) {
	    if( icmWkspc->icmFp[ fp ].sp < icmWkspc->icmFp[ fp ].cnt  ) {
		icmWkspc->icmFp[ fp ].msgPoolArray[ icmWkspc->icmFp[ fp ].sp++ ] = icmMsg;
		*err = ICM_NO_ERR;
		icmLog( ICM_LOG_V3, "icmFreeToPool(); Released 0x%08X of size %d to fp %d\n",
			(int *)icmMsg, icmMsg->allocPool, fp );
		break;
	    } else {
		icmErrorLog( "icmFreeToPool(): Free Pool Full on icmMsg->allocPool %d\n",
				icmMsg->allocPool );
		*err = ICM_ERR_FREE;
		break;
	    }
	}
    }
    icmUnlockFp();

    if( *err == ICM_ERR_ALLOC_SIZE ) {
	icmErrorLog( "icmFreeToPool(): Failed to find pool on icmMsg->allocPool %d\n",
			icmMsg->allocPool );
    }
}

void icmInitLocks( int *err )
{
    *err = ICM_ERR_LOCKS;

    if( !icmOsCreateLock( ICM_SEM_FP, 1 ) ) {
	icmErrorLog( "icmInitLocks(): FAIL, ICM_SEM_FP\n" );
	return;
    }

    if( !icmOsCreateLock( ICM_SEM_DISPATCH, 1 ) ) {
	icmErrorLog( "icmInitLocks(): FAIL, ICM_SEM_DISPATCH\n" );
	return;
    }
    
    if( !icmOsCreateLock( ICM_SEM_WKSPC, 1 ) ) {
	icmErrorLog( "icmInitLocks(): FAIL, ICM_SEM_WKSPC\n" );
	return;
    }
    
    *err = ICM_NO_ERR; 
}

void icmLockFp()
{
    if( !icmOsLockAcquire( ICM_SEM_FP ) ) {
	icmErrorLog( "icmLockFp(): icmOsLockAcquire() FAIL\n" );
    }
}

void icmUnlockFp()
{
    if( !icmOsLockRelease( ICM_SEM_FP ) ) {
	icmErrorLog( "icmUnlockFp(): icmOsLockRelease() FAIL\n" );
    }
}

void icmLockDispatch()
{
    if( !icmOsLockAcquire( ICM_SEM_DISPATCH ) ) {
	icmErrorLog( "icmLockDispatch(): icmOsLockAcquire() FAIL\n" );
    }
}

void icmUnlockDispatch()
{
    if( !icmOsLockRelease( ICM_SEM_DISPATCH ) ) {
	icmErrorLog( "icmUnlockDispatch(): icmOsLockRelease() FAIL\n" );
    }
}

void icmLockWorkspace()
{
    if( !icmOsLockAcquire( ICM_SEM_WKSPC ) ) {
	icmErrorLog( "icmLockWorkspace(): icmOsLockAcquire() FAIL\n" );
    }
}

void icmUnlockWorkspace()
{
    if( !icmOsLockRelease( ICM_SEM_WKSPC ) ) {
	icmErrorLog( "icmUnlockWorkspace(): icmOsLockRelease() FAIL\n" );
    }
}

