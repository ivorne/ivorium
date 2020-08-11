#pragma once

#include "ElementRenderer.hpp"
#include "../Rendering/CameraState.hpp"
#include "../Rendering/FlatShader.hpp"

#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

class Camera;

/**
*/
class Elem : public InputNode
{
public:
ClientMarker cm;
                            Elem( Instance * inst );
                            ~Elem();
    
    void                    status( iv::TableDebugView * view );
    
    Instance *              instance() const;


//------------------------- element tree --------------------------------
    /**
    */
    void                    elem_setParent( Elem * );
    
    /**
    */
    Elem *                  elem_getParent();
    
    /**
    */
    virtual Camera *        elem_getRoot();
    
    /**
        Just used for debugging.
    */
    virtual void            elem_eachChild( std::function< void( Elem * ) > const & ){}
    
    /**
    */
    virtual void            elem_childDisconnect( Elem * ){}
    
//------------------------- scene update traversal --------------------------------
    /**
        Refresh synthesized parameters, from bottom to top.
        Also refresh everything (except childrens inherited parameters) that depends on synthesized or initialization parameters and does not depend on inherited parameters.
    
        For each child:
            - Call child->first_pass().
            - Check changes in own initialization parameters and childs synthesized parameters.
                * If this change would modify own internal state or own synthesized parameters, then modify them.
                    * Changes to internal state caused by this change can also be delayed to second pass, in which case we need to queue for that second pass.
                    * This is useful if we realy want to avoid double recomputation of internal state that can be changed both by initialization / synthesized parameters and inherited parameters.
                * If this change would modify childrens inherited parameters, then queue self for second pass.
                    * No need to queue self for second pass when own inherited parameters changed - inherited parameters are set from parent and parent calls second_pass automatically on child when this happens.
    */
    void                    first_pass( ElementRenderer * );
    
    /**
        Refresh inherited parameters and whatever depends on them, from top to bottom.
        
        Remove self from queue for second pass (necessary only if there is a possibility that it added self in that queue in first pass, not necessary when called from parents second_pass).
        Regenerate internal state and childrens inherited parameters if they depend on changed inherited parameters.
        Also regenerate internal states that depend on initialization or synthesized parameters and the regeneration was delayed to second pass.
        If we changed at leas one synthesized parameter in a child, then we need to call second_pass on that child too (this should be called once after all desired changes to that childs inherited parameters were made).
    */
    void                    second_pass( ElementRenderer * );
    
    //
    unsigned                first_pass_frame_id();
    unsigned                second_pass_frame_id();
    
//------------------------- parameters --------------------------------
// initialization parameters
    /**
        Elements are enabled by default.
        Disabled Elements do not call first_pass or second_pass and therefore no children of them are processed or drawn.
        This also sets InputNode::inputEnabled to the same value during first pass.
    */
    DirtyAttr< bool >       attr_enabled;
    
// inherited parameter
    DirtyAttr< float4x4 >   modelTransform;
    DirtyAttr< ShaderScissor > scissor;

// utility methods
    Elem *                  enabled( bool val );
    
    /**
        This node being disconnected from scene tree will not be reported in ConsistencyChecks log.
    */
    void                    quiet( bool );
    bool                    quiet() const;
    
//------------------------- utilities --------------------------------
// math over modelTransform
    float3                  FromLocalSpaceToScreenSpace( float3 local_space );
    float3                  FromScreenSpaceToLocalSpace( float3 screen_space );
    float2                  FromScreenPlaneToLocalPlane( float2 screen_space );

// input tree utilities
    void                    Add_InputNode( InputNode * node );
    void                    Remove_InputNode( InputNode * node );

protected:
    // InputNode
    virtual void            input_childDisconnect( InputNode * ) override;
    virtual void            input_eachChild( std::function< void( InputNode * ) > const & ) override;

    // Elem
    virtual void            first_pass_impl( ElementRenderer * ) = 0;
    virtual void            second_pass_impl( ElementRenderer * ){}
    
private:
    Instance * inst;
    unsigned _first_pass_frame_id;
    unsigned _second_pass_frame_id;
    bool _quiet;
    std::vector< InputNode * > _input_children;
    Elem * _elem_parent;
};

class Pickable
{
public:
ClientMarker cm;

                            Pickable( Elem * elem );
    virtual bool            picking_test( int2 input_pos ) = 0;
    
    Elem *                  elem();
    Elem const *            elem() const;
    
private:
    Elem * _elem;
};

}
