#include "GlError.hpp"

#include "gl.h"
#include <ivorium_core/ivorium_core.hpp>
#include <ivorium_config.hpp>

namespace iv
{

void GlError_Check( Context const * logger, SrcInfo const & src_info )
{
    GLenum err = glGetError();
    if( err != GL_NO_ERROR )
    {
        char const * str = nullptr;
        switch( err )
        {
            case GL_INVALID_ENUM:       str = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE:      str = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:  str = "GL_INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY:      str = "GL_OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: str = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            /*
#if IV_GLPLATFORM_GLFM
            case GL_STACK_OVERFLOW:     str = "GL_STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:    str = "GL_STACK_UNDERFLOW"; break;
            case GL_CONTEXT_LOST:       str = "GL_CONTEXT_LOST"; break;
#endif*/
        }
        
        if( str )
            logger->log( src_info, iv::Defs::Log::Warning, "OpenGL error: ", str, "." );
        else
            logger->log( src_info, iv::Defs::Log::Warning, "Unknown OpenGL error: ", err, "." );
    }
}

}
