#include        <stdint.h>
#include        <stdlib.h>
#include        <icm_pub.h>
#include        <icm_private.h>

/*  FreeP Pool Size Specifiers.
 *
 *  Configure with ascending sizes to ensure proper search
 *  at runtime.
 */
ICM_FP_CONFIG fpConfig[ ICM_MAX_FREE_POOL ] = {

   /* Size   Count */
   {    512,   512   },
   {   2048,     8   },

};

