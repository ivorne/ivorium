#pragma once

#include "gl.h"
#include <ivorium_core/ivorium_core.hpp>
#include <string>


namespace iv
{

/**
 Thin wrapper over ofShader.
*/
class GlProgram
{
public:
                                GlProgram();

    //------------------------- loading ------
    void                        CreateProgram( ClientMarker const * logger );
    void                        Load_VertexShader( ClientMarker const * logger, std::istream & in );
    void                        Load_FragmentShader( ClientMarker const * logger, std::istream & in );
    void                        BindAttribute( ClientMarker const * logger, GLuint location, const char * attrib_name );
    void                        PositionAttributeName( ClientMarker const * logger, const char * name ); ///< Used by transform feedback. Must be vec4.
    void                        LinkProgram( ClientMarker const * logger );
    void                        DestroyProgram( ClientMarker const * logger );
    void                        DropProgram( ClientMarker const * logger );
    
    //------------------------- getters -------------------
    GLuint                      program_id() const;
    GLint                       GetUniformLocation( const char * name ) const;
    
private:
    static std::string          ExtractShaderInfoLog( GLuint shader );
    static std::string          ExtractProgramInfoLog( GLuint program );
    
private:
    GLuint _program_id;
    bool _linked;
    GLuint _vertex_id;
    GLuint _fragment_id;
};

}
