#include "GlfwWindow.hpp"
#include <ivorium_config.hpp>

#if IV_GLPLATFORM_GLFW

#include <ivorium_core/ivorium_core.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iomanip>
#include <unistd.h>

namespace iv
{

GlfwWindow::GlfwWindow( std::string const & window_title ) :
    window( nullptr ),
    listener( nullptr ),
    _render_target(),
    _time_last( glfwGetTime() ),
    _focused( true ),
    _current_character( 0 ),
    _floating_fps( 0.0 )
{
    // window hints
    glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
    glfwWindowHint( GLFW_SCALE_TO_MONITOR, GLFW_TRUE );
    glfwWindowHint( GLFW_CENTER_CURSOR, GLFW_FALSE );
    //glfwWindowHint( GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE );
    //glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );
    
    // framebuffer hints
    glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
    glfwWindowHint( GLFW_RED_BITS, 8 );
    glfwWindowHint( GLFW_GREEN_BITS, 8 );
    glfwWindowHint( GLFW_BLUE_BITS, 8 );
    glfwWindowHint( GLFW_ALPHA_BITS, 8 );
    glfwWindowHint( GLFW_SAMPLES, 1 );
    //glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
    
    // monitor hints
    //glfwWindowHint( GLFW_REFRESH_RATE, 60 );
    
    // context hints
    glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    
    // create window
    this->window = glfwCreateWindow( 365, 650, window_title.c_str(), NULL, NULL );
    if( !this->window )
    {
        runtime_warning( SRC_INFO, "Call to glfwCreateWindow failed." );
        return;
    }
    
    // read window geometry
    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    xscale = 1.0f;
    yscale = 1.0f;
    
    RenderTarget::Geometry geo;
    glfwGetWindowSize(window, &geo.size.x, &geo.size.y);
    geo.density = ( xscale + yscale ) / 2.0f;
    
    this->_render_target.set_geometry( geo );
    
    // opengl setup
    glfwMakeContextCurrent( this->window );
    glewExperimental = GL_TRUE; 
    glewInit();
    this->setupGL();
    glfwSetWindowSizeLimits( this->window, 50, 50, GLFW_DONT_CARE, GLFW_DONT_CARE );
    
    // set callbacks
    glfwSetWindowUserPointer( this->window, reinterpret_cast< void * >( this ) );
    glfwSetWindowFocusCallback( this->window, GlfwWindow::S_FocusCallback );
    glfwSetWindowContentScaleCallback( this->window, GlfwWindow::S_ContentScaleCallback );
    glfwSetWindowSizeCallback( this->window, GlfwWindow::S_SizeCallback );
    glfwSetFramebufferSizeCallback( this->window, GlfwWindow::S_FBSizeCallback );
    glfwSetKeyCallback( this->window, GlfwWindow::S_KeyCallback );
    glfwSetMouseButtonCallback( this->window, GlfwWindow::S_MouseButtonCallback );
    glfwSetScrollCallback( this->window, GlfwWindow::S_ScrollCallback );
    glfwSetCharCallback( this->window, GlfwWindow::S_CharCallback );
}

GlfwWindow::~GlfwWindow()
{
    glfwDestroyWindow( this->window );
}

bool GlfwWindow::gpu_enabled()
{
    return true;
}

void GlfwWindow::set_listener( WindowListener * listener )
{
    this->listener = listener;
}

RenderTarget::Geometry GlfwWindow::geometry()
{
    return this->_render_target.geometry();
}

RenderTarget * GlfwWindow::render_target()
{
    return &this->_render_target;
}

void GlfwWindow::loop()
{
    while( !glfwWindowShouldClose( this->window ) )
    {
        // compute delta time
        double time = glfwGetTime();
        double delta = time - this->_time_last;
        uint64_t delta_ms = delta * 1000.0;
        this->_time_last = time;
        
        // show FPS
        #ifndef NDEBUG
        {
            double fps = 1.0 / delta;
            this->_floating_fps = this->_floating_fps * 0.9 + fps * 0.1;
            
            std::stringstream ss;
            ss << std::setprecision( 1 ) << std::fixed << this->_floating_fps << " fps";
            glfwSetWindowTitle( this->window, ss.str().c_str() );
        }
        #endif
        
        // events
        glfwPollEvents();
        
        // update
        this->listener->update( delta_ms );
        
        // render
        this->render();
        
        //
        double target_fps = double( glfwGetVideoMode( glfwGetPrimaryMonitor() )->refreshRate );
        double frame_end = time + 1.0 / target_fps;
        
        // extra update
        bool more_extras_needed = this->listener->extra_update(); // at least one extra update
        while( more_extras_needed )
        {
            // check if we have time
            if( glfwGetTime() >= frame_end )
                break;
            
            // call second extra update
            more_extras_needed = this->listener->extra_update();
        }
        
        // sleep until next frame
        double after_time = glfwGetTime();
        double us_remaining = ( frame_end - glfwGetTime() - 0.0005 ) * 1000000.0f;
        if( us_remaining > 0 )
            usleep( (int)us_remaining );
        
        // precise sync
        while( glfwGetTime() < frame_end );
    }
}

void GlfwWindow::render()
{
    // render
    glGetError();   // clear errors
    glClear( GL_COLOR_BUFFER_BIT );
    
    this->_render_target.frame_setup();
    this->listener->draw();
    this->_render_target.frame_close();
    
    glfwSwapBuffers( this->window );
}

void GlfwWindow::FocusCallback( int focused )
{
    this->_focused = focused;
    if( !focused )
        this->listener->focus_lost();
}

void GlfwWindow::ContentScaleCallback( float xscale, float yscale )
{
    RenderTarget::Geometry geo = this->_render_target.geometry();
    geo.density = ( xscale + yscale ) / 2.0f;
    this->_render_target.set_geometry( geo );
    this->listener->resized( geo );
}

void GlfwWindow::SizeCallback( int width, int height )
{
    RenderTarget::Geometry geo = this->_render_target.geometry();
    geo.size.x = width;
    geo.size.y = height;
    this->_render_target.set_geometry( geo );
    this->listener->resized( geo );
}

void GlfwWindow::FBSizeCallback( int width, int height )
{
    glViewport( 0, 0, width, height );
}

//---- keyboard -----
void GlfwWindow::KeyCallback( int key, int scancode, int action, int mods )
{
     auto ivkey = this->KeyboardKey_GlfwToIvorium( key );
     if( !ivkey.has_value() )
        return;
        
    Input::Type type;
    switch( action )
    {
        case GLFW_PRESS:    type = Input::Type::Press;      break;
        case GLFW_RELEASE:  type = Input::Type::Release;    break;
        default: return;
    }
    
    Input i( type, ivkey.value(), 0, this->_focused );
    this->listener->input( &i );
}

//---- mouse -----
void GlfwWindow::MouseButtonCallback( int button, int action, int mods )
{
     auto ivbutton = this->MouseButton_GlfwToIvorium( button );
     if( !ivbutton.has_value() )
        return;
        
    Input::Type type;
    switch( action )
    {
        case GLFW_PRESS:    type = Input::Type::Press;      break;
        case GLFW_RELEASE:  type = Input::Type::Release;    break;
        default: return;
    }
    
    Input i( type, ivbutton.value(), 0, this->_focused );
    this->listener->input( &i );
}

void GlfwWindow::ScrollCallback( double xoffset, double yoffset )
{
    while( xoffset > 0.5f )
    {
        Input i( Input::Type::Trigger, Input::Key::MouseScrollLeft, 0, this->_focused );
        this->listener->input( &i );
        xoffset -= 1.0f;
    }
    
    while( xoffset < -0.5f )
    {
        Input i( Input::Type::Trigger, Input::Key::MouseScrollRight, 0, this->_focused );
        this->listener->input( &i );
        xoffset += 1.0f;
    }
    
    while( yoffset > 0.5f )
    {
        Input i( Input::Type::Trigger, Input::Key::MouseScrollUp, 0, this->_focused );
        this->listener->input( &i );
        yoffset -= 1.0f;
    }
    
    while( yoffset < -0.5f )
    {
        Input i( Input::Type::Trigger, Input::Key::MouseScrollDown, 0, this->_focused );
        this->listener->input( &i );
        yoffset += 1.0f;
    }
}

void GlfwWindow::CharCallback( unsigned int codepoint )
{
    this->_current_character = codepoint;
    Input i( Input::Type::Trigger, Input::Key::Character, 0, this->_focused );
    this->listener->input( &i );
    this->_current_character = 0;
}

int2 GlfwWindow::input_position( Input::Key key, int device_id )
{
    if( key >= Input::Key::MOUSE_BEGIN && key < Input::Key::MOUSE_END )
    {
        double xpos, ypos;
        glfwGetCursorPos( this->window, &xpos, &ypos );
        return int2( xpos, ypos );
    }
    
    return int2( 0, 0 );
}

float GlfwWindow::input_value( Input::Key key, int device_id )
{
    return 0.0f;
}

unsigned GlfwWindow::input_character()
{
    return this->_current_character;
}


//===========================================================================================
void GlfwWindow::S_FocusCallback( GLFWwindow * window, int focused )                                { reinterpret_cast< GlfwWindow * >( glfwGetWindowUserPointer( window ) )->FocusCallback( focused ); }
void GlfwWindow::S_ContentScaleCallback( GLFWwindow * window, float xscale, float yscale )          { reinterpret_cast< GlfwWindow * >( glfwGetWindowUserPointer( window ) )->ContentScaleCallback( xscale, yscale ); }
void GlfwWindow::S_SizeCallback( GLFWwindow * window, int width, int height )                       { reinterpret_cast< GlfwWindow * >( glfwGetWindowUserPointer( window ) )->SizeCallback( width, height ); }
void GlfwWindow::S_FBSizeCallback( GLFWwindow * window, int width, int height )                     { reinterpret_cast< GlfwWindow * >( glfwGetWindowUserPointer( window ) )->FBSizeCallback( width, height ); }
void GlfwWindow::S_KeyCallback( GLFWwindow * window, int key, int scancode, int action, int mods )  { reinterpret_cast< GlfwWindow * >( glfwGetWindowUserPointer( window ) )->KeyCallback( key, scancode, action, mods ); }
void GlfwWindow::S_MouseButtonCallback( GLFWwindow * window, int button, int action, int mods )     { reinterpret_cast< GlfwWindow * >( glfwGetWindowUserPointer( window ) )->MouseButtonCallback( button, action, mods ); }
void GlfwWindow::S_ScrollCallback( GLFWwindow * window, double xoffset, double yoffset )            { reinterpret_cast< GlfwWindow * >( glfwGetWindowUserPointer( window ) )->ScrollCallback( xoffset, yoffset ); }
void GlfwWindow::S_CharCallback( GLFWwindow * window, unsigned int codepoint )                      { reinterpret_cast< GlfwWindow * >( glfwGetWindowUserPointer( window ) )->CharCallback( codepoint ); }

//===========================================================================================
std::optional< Input::Key > GlfwWindow::KeyboardKey_GlfwToIvorium( int key )
{
    switch( key )
    {
        case GLFW_KEY_SPACE:        return Input::Key::Space;
        case GLFW_KEY_APOSTROPHE:   return Input::Key::Apostrophe;
        case GLFW_KEY_COMMA:        return Input::Key::Comma;
        case GLFW_KEY_MINUS:        return Input::Key::Minus;
        case GLFW_KEY_PERIOD:       return Input::Key::Period;
        case GLFW_KEY_SLASH:        return Input::Key::Slash;
        case GLFW_KEY_0:            return Input::Key::Num_0;
        case GLFW_KEY_1:            return Input::Key::Num_1;
        case GLFW_KEY_2:            return Input::Key::Num_2;
        case GLFW_KEY_3:            return Input::Key::Num_3;
        case GLFW_KEY_4:            return Input::Key::Num_4;
        case GLFW_KEY_5:            return Input::Key::Num_5;
        case GLFW_KEY_6:            return Input::Key::Num_6;
        case GLFW_KEY_7:            return Input::Key::Num_7;
        case GLFW_KEY_8:            return Input::Key::Num_8;
        case GLFW_KEY_9:            return Input::Key::Num_9;
        case GLFW_KEY_SEMICOLON:    return Input::Key::Semicolon;
        case GLFW_KEY_EQUAL:        return Input::Key::Equal;
        case GLFW_KEY_A:            return Input::Key::Char_A;
        case GLFW_KEY_B:            return Input::Key::Char_B;
        case GLFW_KEY_C:            return Input::Key::Char_C;
        case GLFW_KEY_D:            return Input::Key::Char_D;
        case GLFW_KEY_E:            return Input::Key::Char_E;
        case GLFW_KEY_F:            return Input::Key::Char_F;
        case GLFW_KEY_G:            return Input::Key::Char_G;
        case GLFW_KEY_H:            return Input::Key::Char_H;
        case GLFW_KEY_I:            return Input::Key::Char_I;
        case GLFW_KEY_J:            return Input::Key::Char_J;
        case GLFW_KEY_K:            return Input::Key::Char_K;
        case GLFW_KEY_L:            return Input::Key::Char_L;
        case GLFW_KEY_M:            return Input::Key::Char_M;
        case GLFW_KEY_N:            return Input::Key::Char_N;
        case GLFW_KEY_O:            return Input::Key::Char_O;
        case GLFW_KEY_P:            return Input::Key::Char_P;
        case GLFW_KEY_Q:            return Input::Key::Char_Q;
        case GLFW_KEY_R:            return Input::Key::Char_R;
        case GLFW_KEY_S:            return Input::Key::Char_S;
        case GLFW_KEY_T:            return Input::Key::Char_T;
        case GLFW_KEY_U:            return Input::Key::Char_U;
        case GLFW_KEY_V:            return Input::Key::Char_V;
        case GLFW_KEY_W:            return Input::Key::Char_W;
        case GLFW_KEY_X:            return Input::Key::Char_X;
        case GLFW_KEY_Y:            return Input::Key::Char_Y;
        case GLFW_KEY_Z:            return Input::Key::Char_Z;
        case GLFW_KEY_LEFT_BRACKET: return Input::Key::Left_Bracket;
        case GLFW_KEY_BACKSLASH:    return Input::Key::Backslash;
        case GLFW_KEY_RIGHT_BRACKET:return Input::Key::Right_Bracket;
        case GLFW_KEY_GRAVE_ACCENT: return Input::Key::Grave_Accent;
        case GLFW_KEY_WORLD_1:      return Input::Key::World_1;
        case GLFW_KEY_WORLD_2:      return Input::Key::World_2;
        case GLFW_KEY_ESCAPE:       return Input::Key::Escape;
        case GLFW_KEY_ENTER:        return Input::Key::Enter;
        case GLFW_KEY_TAB:          return Input::Key::Tab;
        case GLFW_KEY_BACKSPACE:    return Input::Key::Backspace;
        case GLFW_KEY_INSERT:       return Input::Key::Insert;
        case GLFW_KEY_DELETE:       return Input::Key::Delete;
        case GLFW_KEY_RIGHT:        return Input::Key::Right;
        case GLFW_KEY_LEFT:         return Input::Key::Left;
        case GLFW_KEY_DOWN:         return Input::Key::Down;
        case GLFW_KEY_UP:           return Input::Key::Up;
        case GLFW_KEY_PAGE_UP:      return Input::Key::Page_Up;
        case GLFW_KEY_PAGE_DOWN:    return Input::Key::Page_Down;
        case GLFW_KEY_HOME:         return Input::Key::Home;
        case GLFW_KEY_END:          return Input::Key::End;
        case GLFW_KEY_CAPS_LOCK:    return Input::Key::Caps_Lock;
        case GLFW_KEY_SCROLL_LOCK:  return Input::Key::Scroll_Lock;
        case GLFW_KEY_NUM_LOCK:     return Input::Key::Num_Lock;
        case GLFW_KEY_PRINT_SCREEN: return Input::Key::Print_Screen;
        case GLFW_KEY_PAUSE:        return Input::Key::Pause;
        case GLFW_KEY_F1:           return Input::Key::F1;
        case GLFW_KEY_F2:           return Input::Key::F2;
        case GLFW_KEY_F3:           return Input::Key::F3;
        case GLFW_KEY_F4:           return Input::Key::F4;
        case GLFW_KEY_F5:           return Input::Key::F5;
        case GLFW_KEY_F6:           return Input::Key::F6;
        case GLFW_KEY_F7:           return Input::Key::F7;
        case GLFW_KEY_F8:           return Input::Key::F8;
        case GLFW_KEY_F9:           return Input::Key::F9;
        case GLFW_KEY_F10:          return Input::Key::F10;
        case GLFW_KEY_F11:          return Input::Key::F11;
        case GLFW_KEY_F12:          return Input::Key::F12;
        case GLFW_KEY_F13:          return Input::Key::F13;
        case GLFW_KEY_F14:          return Input::Key::F14;
        case GLFW_KEY_F15:          return Input::Key::F15;
        case GLFW_KEY_F16:          return Input::Key::F16;
        case GLFW_KEY_F17:          return Input::Key::F17;
        case GLFW_KEY_F18:          return Input::Key::F18;
        case GLFW_KEY_F19:          return Input::Key::F19;
        case GLFW_KEY_F20:          return Input::Key::F20;
        case GLFW_KEY_F21:          return Input::Key::F21;
        case GLFW_KEY_F22:          return Input::Key::F22;
        case GLFW_KEY_F23:          return Input::Key::F23;
        case GLFW_KEY_F24:          return Input::Key::F24;
        case GLFW_KEY_F25:          return Input::Key::F25;
        case GLFW_KEY_KP_0:         return Input::Key::KeyPad_0;
        case GLFW_KEY_KP_1:         return Input::Key::KeyPad_1;
        case GLFW_KEY_KP_2:         return Input::Key::KeyPad_2;
        case GLFW_KEY_KP_3:         return Input::Key::KeyPad_3;
        case GLFW_KEY_KP_4:         return Input::Key::KeyPad_4;
        case GLFW_KEY_KP_5:         return Input::Key::KeyPad_5;
        case GLFW_KEY_KP_6:         return Input::Key::KeyPad_6;
        case GLFW_KEY_KP_7:         return Input::Key::KeyPad_7;
        case GLFW_KEY_KP_8:         return Input::Key::KeyPad_8;
        case GLFW_KEY_KP_9:         return Input::Key::KeyPad_9;
        case GLFW_KEY_KP_DECIMAL:   return Input::Key::KeyPad_Decimal;
        case GLFW_KEY_KP_DIVIDE:    return Input::Key::KeyPad_Divide;
        case GLFW_KEY_KP_MULTIPLY:  return Input::Key::KeyPad_Multiply;
        case GLFW_KEY_KP_SUBTRACT:  return Input::Key::KeyPad_Subtract;
        case GLFW_KEY_KP_ADD:       return Input::Key::KeyPad_Add;
        case GLFW_KEY_KP_ENTER:     return Input::Key::KeyPad_Enter;
        case GLFW_KEY_KP_EQUAL:     return Input::Key::KeyPad_Equal;
        case GLFW_KEY_LEFT_SHIFT:   return Input::Key::Left_Shift;
        case GLFW_KEY_LEFT_CONTROL: return Input::Key::Left_Control;
        case GLFW_KEY_LEFT_ALT:     return Input::Key::Left_Alt;
        case GLFW_KEY_LEFT_SUPER:   return Input::Key::Left_Super;
        case GLFW_KEY_RIGHT_SHIFT:  return Input::Key::Right_Shift;
        case GLFW_KEY_RIGHT_CONTROL:return Input::Key::Right_Control;
        case GLFW_KEY_RIGHT_ALT:    return Input::Key::Right_Alt;
        case GLFW_KEY_RIGHT_SUPER:  return Input::Key::Right_Super;
        case GLFW_KEY_MENU:         return Input::Key::Menu;
        default: return std::nullopt;
    }
}

std::optional< Input::Key > GlfwWindow::MouseButton_GlfwToIvorium( int button )
{
    switch( button )
    {
        case GLFW_MOUSE_BUTTON_LEFT:    return Input::Key::MouseLeft;
        case GLFW_MOUSE_BUTTON_RIGHT:   return Input::Key::MouseRight;
        case GLFW_MOUSE_BUTTON_MIDDLE:  return Input::Key::MouseMiddle;
        case GLFW_MOUSE_BUTTON_4:       return Input::Key::MouseBack;
        case GLFW_MOUSE_BUTTON_5:       return Input::Key::MouseForward;
        default: return std::nullopt;
    }
}

}

#endif
