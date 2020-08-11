#pragma once

#include "TimeSystem/TimeId.hpp"
#include "InputSystem/InputBindingSystem/InputId.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{
namespace Defs
{

namespace Log
{
    extern const LogId TimeUpdates;
    
    extern const LogId StreamResources;
    
    extern const LogId Input;
    extern const LogId InputTree;
    extern const LogId InputEvent;
    extern const LogId Picking;
    
    extern const LogId DelayedLoad;
}

namespace Time
{
    extern const TimeId Default;
}

}
}
