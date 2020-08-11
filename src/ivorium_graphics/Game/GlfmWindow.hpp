#pragma once

#include "WindowListener.hpp"
#include "Window.hpp"
#include "../OpenGL/RenderTarget.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <optional>
#include <unordered_map>

class GLFMDisplay;

#if IV_GLPLATFORM_GLFM
#include <glfm.h>
#endif

namespace iv
{

class GlfmRenderTarget : public RenderTarget
{
public:
    using RenderTarget::set_geometry;
    using RenderTarget::frame_setup;
    using RenderTarget::frame_close;
};

class GlfmWindow : public Window
{
public:
                                    GlfmWindow( GLFMDisplay * display );
                                    ~GlfmWindow();
    
    virtual void                    set_listener( WindowListener * listener ) override;
    virtual RenderTarget::Geometry  geometry() override;
    virtual RenderTarget *          render_target() override;
    virtual bool                    gpu_enabled() override;
    
    // InputSource
    virtual int2                    input_position( Input::Key key, int device_id ) override;
    virtual float                   input_value( Input::Key key, int device_id ) override;
    virtual unsigned                input_character() override;
    
private:
#if IV_GLPLATFORM_GLFM
    static void                     s_onSurfaceCreated( GLFMDisplay * display, int width, int height );
    void                            onSurfaceCreated( int width, int height );
    
    static void                     s_onSurfaceDestroyed( GLFMDisplay * display );
    void                            onSurfaceDestroyed();
    
    static void                     s_onSurfaceResized( GLFMDisplay * display, int width, int height );
    void                            onSurfaceResized( int width, int height );
    
    static bool                     s_TouchFunc( GLFMDisplay * display, int touch, GLFMTouchPhase phase, double x, double y );
    bool                            TouchFunc( int touch, GLFMTouchPhase phase, double x, double y );
    
    static void                     s_SurfaceErrorFunc( GLFMDisplay * display, const char * message );
    void                            SurfaceErrorFunc( const char * message );
    
    static void                     s_MemoryWarningFunc( GLFMDisplay * display );
    void                            MemoryWarningFunc();
    
    static void                     s_FocusFunc( GLFMDisplay * display, bool focused );
    void                            FocusFunc( bool focused );
    
    static void                     s_onFrame( GLFMDisplay * display, double frameTime_s );
    void                            onFrame( double frameTime_s );
    
    static bool                     s_KeyFunc( GLFMDisplay * display, GLFMKey keyCode, GLFMKeyAction action, int modifiers );
    bool                            KeyFunc( GLFMKey keyCode, GLFMKeyAction action, int modifiers );
    
    static void                     s_CharFunc( GLFMDisplay * display, const char * utf8, int modifiers );
    void                            CharFunc( const char * utf8, int modifiers );
    
    static bool                     s_EmscriptenKeyFunc( GLFMDisplay * display, const char * code, GLFMKeyAction action, int modifiers );
    bool                            EmscriptenKeyFunc( const char * code, GLFMKeyAction action, int modifiers );
#endif

private:
    void                            render();
    void                            PopulateEmscriptenKeyMap();
    
private:
    GLFMDisplay *                   display;

    //
    WindowListener *                listener;
    
    //
    GlfmRenderTarget                _render_target;
    
    //
    double                          _lastFrameTime_s;
    
    // input
    std::unordered_map< int, int2 > _touches;
    std::unordered_map< std::string, Input::Key > emscriptenKeyMap;
    unsigned                        _current_character;
    
    //
    bool                            _gpu;
};

}
