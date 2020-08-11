#include "Defs.hpp"

namespace iv
{
namespace Defs
{

namespace Log
{
    const LogId TimeUpdates = LogId::create( "TimeUpdates" );
    const LogId StreamResources = LogId::create( "StreamResources" );
    const LogId Input = LogId::create( "Input" );
    const LogId InputTree = LogId::create( "InputTree" );
    const LogId InputEvent = LogId::create( "InputEvent" );
    const LogId Picking = LogId::create( "Picking" );
    const LogId DelayedLoad = LogId::create( "DelayedLoad" );
}

namespace Time
{
    const TimeId Default = TimeId::create( "Default" );
}

}
}
