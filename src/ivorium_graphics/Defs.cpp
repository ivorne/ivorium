#include "Defs.hpp"

namespace iv
{
namespace Defs
{

namespace Log
{
    const LogId Animation_Activations = LogId::create( "Animation_Activations" );
    const LogId Animation_SystemUpdate = LogId::create( "Animation_SystemUpdate" );
    const LogId Animation_NodeUpdate = LogId::create( "Animation_NodeUpdate" );
    const LogId Animation_ConnectorUpdate = LogId::create( "Animation_ConnectorUpdate" );
    const LogId Animation_Events = LogId::create( "Animation_Events" );
    const LogId Animation_Summary = LogId::create( "Animation_Summary" );
    
    const LogId Render = LogId::create( "Render" );
    const LogId TransformFeedback = LogId::create( "TransformFeedback" );
    
    const LogId ElementRefreshSummary = LogId::create( "ElementRefreshSummary" );
    const LogId ElementRenderSummary = LogId::create( "ElementRenderSummary" );
    const LogId ElementFirstPass = LogId::create( "ElementFirstPass" );
    const LogId ElementSecondPass = LogId::create( "ElementSecondPass" );
    const LogId ElementRenderPass = LogId::create( "ElementRenderPass" );
    const LogId ElementFirstPass_Refresh = LogId::create( "ElementFirstPass_Refresh" );
    
    const LogId Text_Verbose = LogId::create( "Text_Verbose" );
    
    const LogId FixedOrderCamera = LogId::create( "FixedOrderCamera" );
    const LogId TextureState = LogId::create( "TextureState" );
}

}
}
