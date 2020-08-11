#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

struct CameraState
{
    float4x4 projection;
    float4x4 pixelizing_projection;     ///< Orthogonal projection matrix with near plane 0 and far plane 1. We use it to get into screen space. Used in UniversalShader::FilteringStage::Msdf and in pixel fitting.
    float4x4 view;
    
    CameraState() : projection( 1 ), pixelizing_projection( 1 ), view( 1 ){}
};

}
