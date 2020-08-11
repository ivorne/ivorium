#include "Window.hpp"
#include <ivorium_config.hpp>

namespace iv
{

void Window::setupGL()
{
    // blending
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    // culling
    glDisable( GL_CULL_FACE );
    //glEnable( GL_CULL_FACE );
    //glFrontFace( GL_CCW );
    //glCullFace( GL_BACK );
    
    // texture transfer
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    
    // primitive restart
#if !IV_WEBGL_USED
    glEnable( GL_PRIMITIVE_RESTART_FIXED_INDEX );
#endif
    
    //
    //ofDisableAntiAliasing();
    //ofBackground( 255,255,255 );
    
    // sRGB
#if !IV_GLPLATFORM_GLFM
    glEnable( GL_FRAMEBUFFER_SRGB );
#endif
}

}
