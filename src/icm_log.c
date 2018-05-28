#include	<stdint.h>
#include	<stdio.h>
#include	<stdarg.h>
#include	<strings.h>
#include	<icm_pub.h>
#include	<icm_private.h>

extern   ICM_WORKSPACE  *icmWkspc;

void icmSetDebugLevel( int level )
{
    icmWkspc->debug = level;
}

int icmGetDebugLevel()
{
    return( icmWkspc->debug );
}

void icmLog( int logLevel, const char *msg, ... )
{
va_list args;

    if( logLevel <= icmWkspc->debug ) {
        va_start( args, msg );
        vfprintf( stdout, msg, args );
        va_end( args );
    }
}

void icmErrorLog( const char *msg, ... )
{
va_list args;
    va_start( args, msg );
    vfprintf( stderr, msg, args );
    va_end( args );
}
