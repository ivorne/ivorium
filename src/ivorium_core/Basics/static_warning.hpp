#pragma once

#include "SrcInfo.hpp"

namespace iv
{

/**
 * Warnings in constructors of static instances, they run before main and are very deterministic.
 */
void startup_warning( SrcInfo info, const char * message );

/**
 * Warnings in runtime that can not be easily associated with a ivorium::Context.
 * Try to use this as rarely as possible since runtime warnings are sometimes unpredictable and it is easier to debug warnings reported to ivorium::Context.
*/
void runtime_warning( SrcInfo info, const char * message );

}
