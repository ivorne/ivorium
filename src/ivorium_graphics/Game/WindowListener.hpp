#pragma once

#include "../OpenGL/RenderTarget.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
 * \ingroup Game
 */
class WindowListener
{
public:

    virtual                     ~WindowListener(){};
    
    /**
    *   Called when input to a Window occured.
    *   \returns True when input was processed by someone.
    */
    virtual bool                input( const Input * input ) = 0;
    
    /**
    */
    virtual void                focus_lost() = 0;
    
    /**
    *    Called with frequency specified in class Window.
    */
    virtual void                update( uint64_t delta_ms ) = 0;
    
    /**
        When there is some extra time remaining in the frame, this will be called.
        Returns true if someting was done so that window will consider calling it again in the same frame.
    */
    virtual bool                extra_update() = 0;
    
    /**
    */
    virtual void                draw() = 0;
    
    /**
        Called when gpu state changed - it got released or attached again.
    */
    virtual void                gpu( bool enabled, bool dropped ) = 0;
    
    /**
     * Called when window size is changed.
     * Not called on app init (only when window size is changed).
     * Physical resolution is winsize * pixel_density.
     * Uusal values for pixel_density are:
        LDPI:    0.75
        MDPI:    1.0
        HDPI:    1.5
        XHDPI:   2.0
        XXHDPI:  3.0
        XXXHDPI: 4.0
     */
    virtual void                resized( RenderTarget::Geometry ) = 0;
};

}
