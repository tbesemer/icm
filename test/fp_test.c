#include        <stdint.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <icm_pub.h>

main()
{
int err;
ICM_MSG *icmMsg[ 4 ];

    icmLog( ICM_LOG_ALWAYS, "fp_test(): Calling icm_init()\n" );
    icmInit( &err, ICM_LOG_V3 );
    if( err ) {
        icmLog( ICM_LOG_ALWAYS, "fp_test(): icm_init() FAILED, err = %d\n", err );
	exit( 1 );
    }

    icmMsg[ 0 ] = icmAllocEvent( 200, &err );
    icmMsg[ 1 ] = icmAllocEvent( 800, &err );
    icmMsg[ 2 ] = icmAllocEvent( 4096, &err );

    icmDumpFreePoolStatus();

    icmMsg[ 0 ]->mainEvent = 0xDEAD;

printf( "main(): icmMsg[ 0 ] == 0x%016lX\n", (long unsigned int)icmMsg[ 0 ] );
    icmFreeEvent( icmMsg[ 0 ], &err );
    icmFreeEvent( icmMsg[ 1 ], &err );
    icmFreeEvent( icmMsg[ 0 ], &err );

   
}
