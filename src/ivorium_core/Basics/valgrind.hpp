#pragma once

#include <ivorium_config.hpp>
#include <cassert>

#if IV_ENABLE_VALGRIND
    #include <valgrind/memcheck.h>
    #define IV_MEMCHECK( X ) VALGRIND_CHECK_VALUE_IS_DEFINED( X )
    #define IV_NULLCHECK( X ) IV_NullcheckFn( (void*)X )
#else
    #define IV_MEMCHECK( X )
    #define IV_NULLCHECK( X ) do{}while(0)
#endif


inline void IV_NullcheckFn( void * x )
{
    assert( x != nullptr );
}
