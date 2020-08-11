#include "Defs.hpp"

namespace iv
{
namespace Defs
{

namespace Log
{
    const LogId Warning = LogId::create( "Warning" );
    const LogId UserWarning = LogId::create( "UserWarning" );
    const LogId Performance = LogId::create( "Performance" );
    const LogId Lex = LogId::create( "Lex" );
    const LogId NewFrame = LogId::create( "NewFrame" );
    const LogId ConsistencyChecks = LogId::create( "ConsistencyChecks" );
    const LogId ConsistencyChecks_Frame = LogId::create( "ConsistencyChecks_Frame" );
}

}
}
