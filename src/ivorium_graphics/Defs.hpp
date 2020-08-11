#pragma once

#include <ivorium_core/ivorium_core.hpp>
#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{
namespace Defs
{

namespace Log
{
    extern const iv::LogId Animation_Activations;
    extern const iv::LogId Animation_SystemUpdate;
    extern const iv::LogId Animation_NodeUpdate;
    extern const iv::LogId Animation_ConnectorUpdate;
    extern const iv::LogId Animation_Events;
    extern const iv::LogId Animation_Summary;
    
    extern const iv::LogId Render;
    extern const iv::LogId TransformFeedback;
    
    extern const iv::LogId ElementRefreshSummary;
    extern const iv::LogId ElementRenderSummary;
    extern const iv::LogId ElementFirstPass;
    extern const iv::LogId ElementSecondPass;
    extern const iv::LogId ElementRenderPass;
    extern const iv::LogId ElementFirstPass_Refresh;
    
    extern const iv::LogId FixedOrderCamera;
    extern const iv::LogId TextureState;
    
    extern const iv::LogId Text_Verbose;
}

}
}
