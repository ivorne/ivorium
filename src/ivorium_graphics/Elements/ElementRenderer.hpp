#pragma once

#include "../Rendering/CameraState.hpp"
#include "../OpenGL/gl.h"
#include <ivorium_core/ivorium_core.hpp>
#include <map>
#include <vector>

namespace iv
{

class Elem;

/**
*/
class Renderable
{
public:
    virtual void render( CameraState const & camera, std::optional< float > depth_override ) = 0;
};

/**
*/
class ElementRenderer
{
public:
ClientMarker cm;
                        ElementRenderer( Instance * inst );
    Instance *          instance() const;
    
    //----------------- root control -----------------------------------
    /**
    */
    void                init_frame();
    void                end_frame();
    
    //----------------- first pass --------------------------------
    /**
        Shoud be called from first pass on the way down (before children have first_pass).
        This means that renderable will be rendered in next frame.
        It will not be rendered in following frames if it is not activated again before each of those frames.
        This behavior allows us to remove renderables from render queue without callbacks into ElementRenderer.
        We can put 0 to shader_id and/or primary_texture_id when we don't know the id yet, but that will preven render grouping.
    */
    virtual void        AddRenderable_Solid( Renderable *, GLuint shader_id, GLuint primary_texture_id ) = 0;
    virtual void        AddRenderable_Translucent( Renderable *, GLuint shader_id, GLuint primary_texture_id ) = 0;
    
    
    //----------------- second pass --------------------------------
    /**
        For optimal performance, queue self in first pass after children were put through first pass.
    */
    void                QueueSecondPass( Elem * );
    
    /**
        Must be called in second_pass to mark that the second pass was done.
        For optimal performance, call this at the beginning of second pass and then call second pass on children
            with changed inherited children in opposite order than first pass was called on them (but not all children have to go through second_pass).
    */
    void                DequeueSecondPass( Elem * );
    
//--------------------- stats -----------------------------------------
    /**
        Should be called if there is some recomputation done in the first pass of the element.
    */
    void                Notify_FirstPass_Refresh( Elem * );
    
    /**
        Should be called if there is some recomputation done in the second pass of the element.
    */
    void                Notify_SecondPass_Refresh( Elem * );
    
protected:
    void                RunSecondPasses();
    virtual void        FrameStart() = 0;
    virtual void        RunRender( CameraState const & state ) = 0;
    
private:
    Instance *              inst;
    std::vector< Elem * >   second_pass;
    
    // stats
    unsigned _first_passes;
    unsigned _second_passes;
};

}
