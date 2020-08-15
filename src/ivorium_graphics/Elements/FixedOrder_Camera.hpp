#pragma once

#include "Camera.hpp"

#include <ivorium_core/ivorium_core.hpp>

#include <map>
#include <vector>

namespace iv
{

/**
    \ingroup Elements
    \brief Scene root that renders items in fixed order, no Z sorting.
    
    Renders elements in the order they were added to the ElementRenderer - order they are placed in the element tree.
    This uses depth buffer for opaque elements, so they will not really be rendered in given order, but they will appear as if they were.
    Opaque elements are in fact grouped by shader id and texture id to have less context switches.
*/
class FixedOrder_Camera : public Camera
{
public:
ClientMarker cm;
                            FixedOrder_Camera( Instance *, iv::RenderTarget::Geometry );

    virtual void            FrameStart() override;
    virtual void            AddRenderable_Solid( Renderable *, GLuint shader_id, GLuint primary_texture_id ) override;
    virtual void            AddRenderable_Translucent( Renderable *, GLuint shader_id, GLuint primary_texture_id ) override;
    virtual void            RunRender( CameraState const & state ) override;

private:
    struct RenderId
    {
        // primary identity
        Renderable * renderable;
        
        // sorting
        GLuint shader_id;
        GLuint primary_texture_id;
        
        RenderId() : renderable( nullptr ), shader_id( 0 ), primary_texture_id( 0 ){}
    };
    
    struct SolidInfo
    {
        unsigned frame_id;
        unsigned depth;
    };
    
    struct TranslucentInfo
    {
        RenderId id;
        unsigned depth;
        
        TranslucentInfo() : id(), depth( 0 ){}
        TranslucentInfo( RenderId id, unsigned depth ) : id( id ), depth( depth ){}
    };
    
    struct RenderIdComparator
    {
        bool operator()( RenderId const & left, RenderId const & right ) const;
    };
    
private:
    unsigned _depth_next;
    std::map< RenderId, SolidInfo, RenderIdComparator > solids;
    std::vector< TranslucentInfo > translucents;
};

}
