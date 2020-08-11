#pragma once

#include "../OpenGL/GlTexture.hpp"
#include "../OpenGL/GlProgram.hpp"
#include "../OpenGL/gl.h"
#include "GlSystem.hpp"

#include <ivorium_systems/ivorium_systems.hpp>
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    To create shader using VirtualResourceProvider method, construct it from a path which has fragment shader source at path+".frag" and vertex shader source at path+".vert".
*/
class Shader
{
public:
ClientMarker cm;
                            Shader( Instance * inst, ResourcePath const & path );
                            ~Shader();
    
    Instance *              instance() const;
    
    void                    status( iv::TableDebugView * view );

    //--------------------- loading ----------------------
    void                    LoadProgram();
    void                    BindAttribute( GLuint location, const char * attrib_name );
    void                    PositionAttributeName( const char * name );
    void                    LinkProgram();
    void                    UnloadProgram();
    void                    DropProgram();
    
    //--------------------- usage ------
    GlProgram const *       gl_program() const;
    GLint                   GetUniformLocation( const char * name ) const;
    
private:
    Instance *              inst;
    ResourcePath            _src_path;
    GlProgram               _gl_program;
};


}
