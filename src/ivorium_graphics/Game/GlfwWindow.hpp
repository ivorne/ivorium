#pragma once

#include "WindowListener.hpp"
#include "Window.hpp"
#include "../OpenGL/RenderTarget.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <optional>

class GLFWwindow;

namespace iv
{

class GlfwRenderTarget : public RenderTarget
{
public:
    using RenderTarget::set_geometry;
    using RenderTarget::frame_setup;
    using RenderTarget::frame_close;
};

class GlfwWindow : public Window
{
public:
                                    GlfwWindow( std::string const & window_title );
                                    ~GlfwWindow();
                            
    void                            loop();
    
    virtual void                    set_listener( WindowListener * listener ) override;
    virtual RenderTarget::Geometry  geometry() override;
    virtual RenderTarget *          render_target() override;
    virtual bool                    gpu_enabled() override;
    
    // InputSource
    virtual int2                    input_position( Input::Key key, int device_id ) override;
    virtual float                   input_value( Input::Key key, int device_id ) override;
    virtual unsigned                input_character() override;
    
    // window focus and geometry
    static void             S_FocusCallback( GLFWwindow * window, int focused );
    void                    FocusCallback( int focused );
    
    static void             S_ContentScaleCallback( GLFWwindow * window, float xscale, float yscale );
    void                    ContentScaleCallback( float xscale, float yscale );
    
    static void             S_SizeCallback( GLFWwindow * window, int width, int height );
    void                    SizeCallback( int width, int height );
    
    static void             S_FBSizeCallback( GLFWwindow * window, int width, int height );
    void                    FBSizeCallback( int width, int height );
    
    // keyboard
    static void             S_KeyCallback( GLFWwindow * window, int key, int scancode, int action, int mods );
    void                    KeyCallback( int key, int scancode, int action, int mods );
    
    // mouse
    static void             S_MouseButtonCallback( GLFWwindow * window, int button, int action, int mods );
    void                    MouseButtonCallback( int button, int action, int mods );
    
    static void             S_ScrollCallback( GLFWwindow * window, double xoffset, double yoffset );
    void                    ScrollCallback( double xoffset, double yoffset );
    
    static void             S_CharCallback( GLFWwindow * window, unsigned int codepoint );
    void                    CharCallback( unsigned int codepoint );
    
private:
    std::optional< Input::Key > KeyboardKey_GlfwToIvorium( int key );
    std::optional< Input::Key > MouseButton_GlfwToIvorium( int button );
    
    void                    render();
    
private:
    //--- core members ---
    GLFWwindow *            window;
    WindowListener *        listener;
    
    GlfwRenderTarget        _render_target;
    
    //--- partial states ---
    double                  _time_last;
    
    bool                    _focused;
    unsigned                _current_character;
    
    double                  _floating_fps;
};

}
