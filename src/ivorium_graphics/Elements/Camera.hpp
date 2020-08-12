#pragma once

#include "Slot.hpp"
#include "ElementRenderer.hpp"
#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

/**
    \brief Abstract class that represents root of a scene graph and can render it.
    
    Add \ref SlotChild instances and then render them using \ref Camera::render_scene.
    Camera can use model matrix of one \ref Elem in scene graph as a view matrix - rendering scene from the point of view of that \ref Elem.
    To be fully functional, a child needs to implement all virtual methods of \ref ElementRenderer - it decides on order in which \ref Renderables are rendered.
*/
class Camera : public Slot, private ElementRenderer
{
public:
ClientMarker cm;
using Slot::instance;

                        Camera( Instance *, RenderTarget::Geometry );
                        ~Camera();
    
    void                status( iv::TableDebugView * view );
    
    /**
        \p view
            Model matrix of this node will be used as view matrix for the whole render.
            Can be null to use empty view matrix - it is rendered from point of view of this camera.
    */
    void                render_scene( Elem * view = nullptr );
    
    /**
    */
    void                geometry( RenderTarget::Geometry geometry );
    
    /**
    */
    virtual Camera *    elem_getRoot() override;
    
//----------------- math --------------------------------------
    float3              FromLocalSpaceToViewportSpace( float4x4 model_transform, float3 );
    float3              FromViewportSpaceToLocalSpace( float4x4 model_transform, float3 );
    float2              FromViewportPlaneToLocalPlane( float4x4 model_transform, float2 );
    
private:
    float3              WNormalized( float4 in );
    float3              FromViewportSpaceToCameraSpace( float3 screen_point );
    float3              FromCameraSpaceToViewportSpace( float3 camera_point );
    
private:
    CameraState state;
    float2 viewport_size;
};

}
