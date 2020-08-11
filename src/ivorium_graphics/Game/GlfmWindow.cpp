#include "GlfmWindow.hpp"
#include <ivorium_config.hpp>

#if IV_GLPLATFORM_GLFM

#include <ivorium_core.hpp>

#include <glfm.h>
#include <utf8.h>

namespace iv
{

GlfmWindow::GlfmWindow( GLFMDisplay * display ) :
    display( display ),
    listener( nullptr ),
    _lastFrameTime_s( 0 ),
    _current_character( 0 ),
    _gpu( false )
{
    //----------
    glfmSetDisplayConfig( display,
                          GLFMRenderingAPIOpenGLES3,
                          GLFMColorFormatRGBA8888,
                          GLFMDepthFormat16,
                          GLFMStencilFormatNone,
                          GLFMMultisampleNone );
 
    glfmSetUserData( display, this );
    glfmSetMultitouchEnabled( display, true );
    
    //---- window
    glfmSetSurfaceCreatedFunc( display, GlfmWindow::s_onSurfaceCreated );
    glfmSetSurfaceDestroyedFunc( display, GlfmWindow::s_onSurfaceDestroyed );
    glfmSetSurfaceResizedFunc( display, GlfmWindow::s_onSurfaceResized );
    glfmSetSurfaceErrorFunc( display, GlfmWindow::s_SurfaceErrorFunc );
    glfmSetMemoryWarningFunc( display, GlfmWindow::s_MemoryWarningFunc );
    glfmSetAppFocusFunc( display, GlfmWindow::s_FocusFunc );
    glfmSetMainLoopFunc( display, GlfmWindow::s_onFrame );
    
    //----- input
    glfmSetTouchFunc( display, GlfmWindow::s_TouchFunc );
    glfmSetKeyFunc( display, GlfmWindow::s_KeyFunc );
    glfmSetEmscriptenKeyFunc( display, GlfmWindow::s_EmscriptenKeyFunc );
    glfmSetCharFunc( display, GlfmWindow::s_CharFunc );
    
    PopulateEmscriptenKeyMap();
    
    //---------
    this->_render_target.set_geometry( RenderTarget::Geometry( float2( 0, 0 ), 1.0 ) );
}

GlfmWindow::~GlfmWindow()
{
}

bool GlfmWindow::gpu_enabled()
{
    return this->_gpu;
}

void GlfmWindow::set_listener( WindowListener * listener )
{
    this->listener = listener;
}

RenderTarget::Geometry GlfmWindow::geometry()
{
    return RenderTarget::Geometry( float2( 100, 100 ), 1.0f );
}

RenderTarget * GlfmWindow::render_target()
{
    return &this->_render_target;
}

//------------------------- window ---------------------------------------------------
void GlfmWindow::render()
{
    // render
    glGetError();   // clear errors
    glClear( GL_COLOR_BUFFER_BIT );
    
    this->_render_target.frame_setup();
    this->listener->draw();
    this->_render_target.frame_close();
}

#if defined( GLFM_PLATFORM_ANDROID )
static double _glfmTimeSeconds(struct timespec t) {
    return t.tv_sec + (double)t.tv_nsec / 1e9;
}

static struct timespec _glfmTimeSubstract(struct timespec a, struct timespec b) {
    struct timespec result;
    if (b.tv_nsec > a.tv_nsec) {
        result.tv_sec = a.tv_sec - b.tv_sec - 1;
        result.tv_nsec = 1000000000 - b.tv_nsec + a.tv_nsec;
    } else {
        result.tv_sec = a.tv_sec - b.tv_sec;
        result.tv_nsec = a.tv_nsec - b.tv_nsec;
    }
    return result;
}

static struct timespec _glfmTimeNow() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return t;
}
#endif

void GlfmWindow::onFrame( double frameTime_s )
{
    #if defined( GLFM_PLATFORM_ANDROID )
    auto start = _glfmTimeNow();
    #endif
    
    // compute delta time
    uint64_t delta_ms = uint64_t( ( frameTime_s - this->_lastFrameTime_s ) * 1000.0 );
    this->_lastFrameTime_s = frameTime_s;
    
    // update
    this->listener->update( delta_ms );
    
    // render
    this->render();
        
    // extra update
    #if defined( GLFM_PLATFORM_ANDROID )
    
    bool more_extras_needed = this->listener->extra_update(); // at least one extra update
    while( more_extras_needed )
    {
        // check if we have time
        auto elapsed_s = _glfmTimeSeconds( _glfmTimeSubstract( _glfmTimeNow(), start ) );
        if( elapsed_s >= 1.0/60.0 )
            break;
        
        // call second extra update
        more_extras_needed = this->listener->extra_update();
    }
    
    #else
        #warning "Unimplemented proper extra updates for this platform."
        this->listener->extra_update();
    #endif
}

void GlfmWindow::onSurfaceCreated( int width, int height )
{
    TextOutput << "GlfmWindow: onSurfaceCreated( " << width << ", " << height << " )" << std::endl;
    TextOutput << "    GL_VENDOR : " << glGetString( GL_VENDOR ) << std::endl;
    TextOutput << "    GL_RENDERER : " << glGetString( GL_RENDERER ) << std::endl;
    TextOutput << "    GL_VERSION : " << glGetString( GL_VERSION ) << std::endl;
    TextOutput << "    GL_SHADING_LANGUAGE_VERSION : " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;
    
    this->_gpu = true;
    this->setupGL();
    this->onSurfaceResized( width, height );
    this->listener->gpu( true, true );
}

void GlfmWindow::onSurfaceDestroyed()
{
    TextOutput << "GlfmWindow: onSurfaceDestroyed" << std::endl;
    this->_gpu = false;
    this->listener->gpu( false, true );
}

void GlfmWindow::onSurfaceResized( int width, int height )
{
    glViewport( 0, 0, width, height );
    
    double scale = glfmGetDisplayScale( this->display );
    if( scale < 0.01 )
        scale = 1.0;
    this->_render_target.set_geometry( RenderTarget::Geometry( int2( width, height ), scale ) );
    
    this->listener->resized( this->_render_target.geometry() );
}

void GlfmWindow::SurfaceErrorFunc( const char * message )
{
    runtime_warning( SRC_INFO, "Glfm surface error:" );
    runtime_warning( SRC_INFO, message );
}

void GlfmWindow::MemoryWarningFunc()
{
    runtime_warning( SRC_INFO, "Glfm memory warning." );
}

void GlfmWindow::FocusFunc( bool focused )
{
    if( !focused )
        this->listener->focus_lost();
}

//----------- input ---------------------------------------------------------------------
int2 GlfmWindow::input_position( Input::Key key, int device_id )
{
    if( key != Input::Key::Touch )
        return int2( 0, 0 );
    
    auto it = this->_touches.find( device_id );
    if( it == this->_touches.end() )
        return int2( 0, 0 );
    
    return it->second;
}

float GlfmWindow::input_value( Input::Key key, int device_id )
{
    return 0.0f;
}

unsigned GlfmWindow::input_character()
{
    return this->_current_character;
}

bool GlfmWindow::TouchFunc( int touch, GLFMTouchPhase phase, double x, double y )
{
    this->_touches[ touch ] = int2( x, y );
    
    if( phase == GLFMTouchPhaseBegan )
    {
        Input i( Input::Type::Press, Input::Key::Touch, touch, true );
        this->listener->input( &i );
    }
    else if( phase == GLFMTouchPhaseEnded )
    {
        Input i( Input::Type::Release, Input::Key::Touch, touch, true );
        this->listener->input( &i );
    }
    else if( phase == GLFMTouchPhaseCancelled )
    {
        Input i( Input::Type::Release, Input::Key::Touch, touch, false );
        this->listener->input( &i );
    }
    
    return false;
}

void GlfmWindow::PopulateEmscriptenKeyMap()
{
    this->emscriptenKeyMap[ "Space" ] = Input::Key::Space;
    this->emscriptenKeyMap[ "Comma" ] = Input::Key::Comma;
    this->emscriptenKeyMap[ "Minus" ] = Input::Key::Minus;
    this->emscriptenKeyMap[ "Period" ] = Input::Key::Period;
    this->emscriptenKeyMap[ "Digit0" ] = Input::Key::Num_0;
    this->emscriptenKeyMap[ "Digit1" ] = Input::Key::Num_1;
    this->emscriptenKeyMap[ "Digit2" ] = Input::Key::Num_2;
    this->emscriptenKeyMap[ "Digit3" ] = Input::Key::Num_3;
    this->emscriptenKeyMap[ "Digit4" ] = Input::Key::Num_4;
    this->emscriptenKeyMap[ "Digit5" ] = Input::Key::Num_5;
    this->emscriptenKeyMap[ "Digit6" ] = Input::Key::Num_6;
    this->emscriptenKeyMap[ "Digit7" ] = Input::Key::Num_7;
    this->emscriptenKeyMap[ "Digit8" ] = Input::Key::Num_8;
    this->emscriptenKeyMap[ "Digit9" ] = Input::Key::Num_9;
    this->emscriptenKeyMap[ "Semicolon" ] = Input::Key::Semicolon;
    this->emscriptenKeyMap[ "Equal" ] = Input::Key::Equal;
    this->emscriptenKeyMap[ "KeyA" ] = Input::Key::Char_A;
    this->emscriptenKeyMap[ "KeyB" ] = Input::Key::Char_B;
    this->emscriptenKeyMap[ "KeyC" ] = Input::Key::Char_C;
    this->emscriptenKeyMap[ "KeyD" ] = Input::Key::Char_D;
    this->emscriptenKeyMap[ "KeyE" ] = Input::Key::Char_E;
    this->emscriptenKeyMap[ "KeyF" ] = Input::Key::Char_F;
    this->emscriptenKeyMap[ "KeyG" ] = Input::Key::Char_G;
    this->emscriptenKeyMap[ "KeyH" ] = Input::Key::Char_H;
    this->emscriptenKeyMap[ "KeyI" ] = Input::Key::Char_I;
    this->emscriptenKeyMap[ "KeyJ" ] = Input::Key::Char_J;
    this->emscriptenKeyMap[ "KeyK" ] = Input::Key::Char_K;
    this->emscriptenKeyMap[ "KeyL" ] = Input::Key::Char_L;
    this->emscriptenKeyMap[ "KeyM" ] = Input::Key::Char_M;
    this->emscriptenKeyMap[ "KeyN" ] = Input::Key::Char_N;
    this->emscriptenKeyMap[ "KeyO" ] = Input::Key::Char_O;
    this->emscriptenKeyMap[ "KeyP" ] = Input::Key::Char_P;
    this->emscriptenKeyMap[ "KeyQ" ] = Input::Key::Char_Q;
    this->emscriptenKeyMap[ "KeyR" ] = Input::Key::Char_R;
    this->emscriptenKeyMap[ "KeyS" ] = Input::Key::Char_S;
    this->emscriptenKeyMap[ "KeyT" ] = Input::Key::Char_T;
    this->emscriptenKeyMap[ "KeyU" ] = Input::Key::Char_U;
    this->emscriptenKeyMap[ "KeyV" ] = Input::Key::Char_V;
    this->emscriptenKeyMap[ "KeyW" ] = Input::Key::Char_W;
    this->emscriptenKeyMap[ "KeyX" ] = Input::Key::Char_X;
    this->emscriptenKeyMap[ "KeyY" ] = Input::Key::Char_Y;
    this->emscriptenKeyMap[ "KeyZ" ] = Input::Key::Char_Z;
    this->emscriptenKeyMap[ "BracketLeft" ] = Input::Key::Left_Bracket;
    this->emscriptenKeyMap[ "Backslash" ] = Input::Key::Backslash;
    this->emscriptenKeyMap[ "BracketRight" ] = Input::Key::Right_Bracket;
    this->emscriptenKeyMap[ "Escape" ] = Input::Key::Escape;
    this->emscriptenKeyMap[ "Enter" ] = Input::Key::Enter;
    this->emscriptenKeyMap[ "Tab" ] = Input::Key::Tab;
    this->emscriptenKeyMap[ "Backspace" ] = Input::Key::Backspace;
    this->emscriptenKeyMap[ "Insert" ] = Input::Key::Insert;
    this->emscriptenKeyMap[ "Delete" ] = Input::Key::Delete;
    this->emscriptenKeyMap[ "ArrowRight" ] = Input::Key::Right;
    this->emscriptenKeyMap[ "ArrowLeft" ] = Input::Key::Left;
    this->emscriptenKeyMap[ "ArrowDown" ] = Input::Key::Down;
    this->emscriptenKeyMap[ "ArrowUp" ] = Input::Key::Up;
    this->emscriptenKeyMap[ "PageUp" ] = Input::Key::Page_Up;
    this->emscriptenKeyMap[ "PageDown" ] = Input::Key::Page_Down;
    this->emscriptenKeyMap[ "Home" ] = Input::Key::Home;
    this->emscriptenKeyMap[ "End" ] = Input::Key::End;
    this->emscriptenKeyMap[ "CapsLock" ] = Input::Key::Caps_Lock;
    this->emscriptenKeyMap[ "ScrollLock" ] = Input::Key::Scroll_Lock;
    this->emscriptenKeyMap[ "NumLock" ] = Input::Key::Num_Lock;
    this->emscriptenKeyMap[ "Pause" ] = Input::Key::Pause;
    this->emscriptenKeyMap[ "F1" ] = Input::Key::F1;
    this->emscriptenKeyMap[ "F2" ] = Input::Key::F2;
    this->emscriptenKeyMap[ "F3" ] = Input::Key::F3;
    this->emscriptenKeyMap[ "F4" ] = Input::Key::F4;
    this->emscriptenKeyMap[ "F5" ] = Input::Key::F5;
    this->emscriptenKeyMap[ "F6" ] = Input::Key::F6;
    this->emscriptenKeyMap[ "F7" ] = Input::Key::F7;
    this->emscriptenKeyMap[ "F8" ] = Input::Key::F8;
    this->emscriptenKeyMap[ "F9" ] = Input::Key::F9;
    this->emscriptenKeyMap[ "F10" ] = Input::Key::F10;
    this->emscriptenKeyMap[ "F11" ] = Input::Key::F11;
    this->emscriptenKeyMap[ "F12" ] = Input::Key::F12;
    this->emscriptenKeyMap[ "F13" ] = Input::Key::F13;
    this->emscriptenKeyMap[ "F14" ] = Input::Key::F14;
    this->emscriptenKeyMap[ "F15" ] = Input::Key::F15;
    this->emscriptenKeyMap[ "F16" ] = Input::Key::F16;
    this->emscriptenKeyMap[ "F17" ] = Input::Key::F17;
    this->emscriptenKeyMap[ "F18" ] = Input::Key::F18;
    this->emscriptenKeyMap[ "F19" ] = Input::Key::F19;
    this->emscriptenKeyMap[ "F20" ] = Input::Key::F20;
    this->emscriptenKeyMap[ "F21" ] = Input::Key::F21;
    this->emscriptenKeyMap[ "F22" ] = Input::Key::F22;
    this->emscriptenKeyMap[ "F23" ] = Input::Key::F23;
    this->emscriptenKeyMap[ "F24" ] = Input::Key::F24;
    this->emscriptenKeyMap[ "F25" ] = Input::Key::F25;
    this->emscriptenKeyMap[ "Numpad0" ] = Input::Key::KeyPad_0;
    this->emscriptenKeyMap[ "Numpad1" ] = Input::Key::KeyPad_1;
    this->emscriptenKeyMap[ "Numpad2" ] = Input::Key::KeyPad_2;
    this->emscriptenKeyMap[ "Numpad3" ] = Input::Key::KeyPad_3;
    this->emscriptenKeyMap[ "Numpad4" ] = Input::Key::KeyPad_4;
    this->emscriptenKeyMap[ "Numpad5" ] = Input::Key::KeyPad_5;
    this->emscriptenKeyMap[ "Numpad6" ] = Input::Key::KeyPad_6;
    this->emscriptenKeyMap[ "Numpad7" ] = Input::Key::KeyPad_7;
    this->emscriptenKeyMap[ "Numpad8" ] = Input::Key::KeyPad_8;
    this->emscriptenKeyMap[ "Numpad9" ] = Input::Key::KeyPad_9;
    this->emscriptenKeyMap[ "NumpadDecimal" ] = Input::Key::KeyPad_Decimal;
    this->emscriptenKeyMap[ "NumpadDivide" ] = Input::Key::KeyPad_Divide;
    this->emscriptenKeyMap[ "NumpadMultiply" ] = Input::Key::KeyPad_Multiply;
    this->emscriptenKeyMap[ "NumpadSubtract" ] = Input::Key::KeyPad_Subtract;
    this->emscriptenKeyMap[ "NumpadAdd" ] = Input::Key::KeyPad_Add;
    this->emscriptenKeyMap[ "NumpadEnter" ] = Input::Key::KeyPad_Enter;
    this->emscriptenKeyMap[ "ShiftLeft" ] = Input::Key::Left_Shift;
    this->emscriptenKeyMap[ "ControlLeft" ] = Input::Key::Left_Control;
    this->emscriptenKeyMap[ "AltLeft" ] = Input::Key::Left_Alt;
    this->emscriptenKeyMap[ "ShiftRight" ] = Input::Key::Right_Shift;
    this->emscriptenKeyMap[ "ControlRight" ] = Input::Key::Right_Control;
    this->emscriptenKeyMap[ "AltRight" ] = Input::Key::Right_Alt;
}

bool GlfmWindow::EmscriptenKeyFunc( const char * code, GLFMKeyAction action, int modifiers )
{
    auto it = this->emscriptenKeyMap.find( code );
    if( it == this->emscriptenKeyMap.end() )
        return false;
    auto key = it->second;
    
    Input::Type type;
    switch( action )
    {
        case GLFMKeyActionPressed:  type = Input::Type::Press;      break;
        case GLFMKeyActionReleased: type = Input::Type::Release;    break;
        default: return false;
    }
    
    Input i( type, key, 0, true );
    return this->listener->input( &i );
}

bool GlfmWindow::KeyFunc( GLFMKey keyCode, GLFMKeyAction action, int modifiers )
{
    return false;
}

void GlfmWindow::CharFunc( const char * value, int modifiers )
{
    if( ivorium::utf8_size( value ) != 1 )
        return;
    
    this->_current_character = ::utf8::next( value, value + strlen( value ) );
    Input i( Input::Type::Trigger, Input::Key::Character, 0, true );
    this->listener->input( &i );
    this->_current_character = 0;
}

//-------------------- static redirects --------------------------------------------------------------------------------
void GlfmWindow::s_onSurfaceCreated( GLFMDisplay * display, const int width, const int height )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    window->onSurfaceCreated( width, height );
}

void GlfmWindow::s_onSurfaceDestroyed( GLFMDisplay * display )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    window->onSurfaceDestroyed();
}

void GlfmWindow::s_onSurfaceResized( GLFMDisplay * display, int width, int height )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    window->onSurfaceResized( width, height );
}

void GlfmWindow::s_onFrame( GLFMDisplay * display, double frameTime_s )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    window->onFrame( frameTime_s );
}

bool GlfmWindow::s_TouchFunc( GLFMDisplay * display, int touch, GLFMTouchPhase phase, double x, double y )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    return window->TouchFunc( touch, phase, x, y );
}

void GlfmWindow::s_SurfaceErrorFunc( GLFMDisplay * display, const char * message )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    window->SurfaceErrorFunc( message );
}

void GlfmWindow::s_MemoryWarningFunc( GLFMDisplay * display )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    window->MemoryWarningFunc();
}

void GlfmWindow::s_FocusFunc( GLFMDisplay * display, bool focused )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    window->FocusFunc( focused );
}

bool GlfmWindow::s_KeyFunc( GLFMDisplay * display, GLFMKey keyCode, GLFMKeyAction action, int modifiers )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    return window->KeyFunc( keyCode, action, modifiers );
}

void GlfmWindow::s_CharFunc( GLFMDisplay * display, const char * utf8, int modifiers )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    window->CharFunc( utf8, modifiers );
}

bool GlfmWindow::s_EmscriptenKeyFunc( GLFMDisplay * display, const char * code, GLFMKeyAction action, int modifiers )
{
    GlfmWindow * window = reinterpret_cast< GlfmWindow * >( glfmGetUserData( display ) );
    return window->EmscriptenKeyFunc( code, action, modifiers );
}

}

#endif
