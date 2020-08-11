#include "GlProgram.hpp"
#include "GlError.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <ivorium_config.hpp>

namespace iv
{

GlProgram::GlProgram() :
    _program_id( 0 ),
    _linked( false ),
    _vertex_id( 0 ),
    _fragment_id( 0 )
{
}

void GlProgram::CreateProgram( ClientMarker const * logger )
{
    if( this->_program_id )
        this->DestroyProgram( logger );
    
    this->_program_id = glCreateProgram();
    
    if( this->_program_id == 0 )
        logger->warning( SRC_INFO, "OpenGL program not created." );
    
    //
    GlError_Check( logger, SRC_INFO );
}

void GlProgram::PositionAttributeName( ClientMarker const * logger, const char * name )
{
    #if IV_ENABLE_TRANSFORM_FEEDBACK
    glTransformFeedbackVaryings( this->_program_id, 1, &name, GL_INTERLEAVED_ATTRIBS );
    GlError_Check( logger, SRC_INFO );
    #endif
}

void GlProgram::Load_VertexShader( ClientMarker const * logger, std::istream & in )
{
    if( !this->_program_id )
        this->CreateProgram( logger );
    
    if( this->_linked )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Program is already linked." );
        return;
    }
    
    if( this->_vertex_id )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Vertex shader already exists in this program, skipping." );
        return;
    }
    
    // read shader source to buffer
    std::string source;
    while( !in.eof() )
    {
        constexpr size_t bufcap = 256;
        char buf[ bufcap ];
        in.read( buf, bufcap );
        source.append( buf, in.gcount() );
    }
    
    char const * src_dta = source.c_str();
    int const src_len = source.size();
    
    // create shader
    this->_vertex_id = glCreateShader( GL_VERTEX_SHADER );
    
    // load source
    glShaderSource( this->_vertex_id, 1, &src_dta, &src_len );
    
    // compile
    glCompileShader( this->_vertex_id );
    
    // check errors
    GLint compiled;
    glGetShaderiv( this->_vertex_id, GL_COMPILE_STATUS, &compiled );
    if( compiled == GL_FALSE )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Vertex shader compilation error: "+GlProgram::ExtractShaderInfoLog( this->_vertex_id ) );
        glDeleteShader( this->_vertex_id );
        this->_vertex_id = 0;
    }
    
    // attach to program
    glAttachShader( this->_program_id, this->_vertex_id );
    
    //
    GlError_Check( logger, SRC_INFO );
}

void GlProgram::Load_FragmentShader( ClientMarker const * logger, std::istream & in )
{
    if( !this->_program_id )
        this->CreateProgram( logger );
    
    if( this->_linked )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Program is already linked." );
        return;
    }
    
    if( this->_fragment_id )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Fragment shader already exists in this program, skipping." );
        return;
    }
    
    // read shader source to buffer
    std::string source;
    while( !in.eof() )
    {
        constexpr size_t bufcap = 256;
        char buf[ bufcap ];
        in.read( buf, bufcap );
        source.append( buf, in.gcount() );
    }
    
    char const * src_dta = source.c_str();
    int const src_len = source.size();
    
    // create shader
    this->_fragment_id = glCreateShader( GL_FRAGMENT_SHADER );
    
    // load source
    glShaderSource( this->_fragment_id, 1, &src_dta, &src_len );
    
    // compile
    glCompileShader( this->_fragment_id );
    
    // check errors
    GLint compiled;
    glGetShaderiv( this->_fragment_id, GL_COMPILE_STATUS, &compiled );
    if( compiled == GL_FALSE )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Fragment shader compilation error: \n"+GlProgram::ExtractShaderInfoLog( this->_fragment_id ) );
        glDeleteShader( this->_fragment_id );
        this->_fragment_id = 0;
    }
    
    // attach to program
    glAttachShader( this->_program_id, this->_fragment_id );
    
    //
    GlError_Check( logger, SRC_INFO );
}

void GlProgram::BindAttribute( ClientMarker const * logger, GLuint location, const char * attrib_name )
{
    if( !this->_program_id )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Program not created." );
        return;
    }
    
    if( this->_linked )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Program is already linked." );
        return;
    }
    
    glBindAttribLocation( this->_program_id, location, attrib_name );
    
    //
    GlError_Check( logger, SRC_INFO );
}

void GlProgram::LinkProgram( ClientMarker const * logger )
{
    if( !this->_program_id )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "No program to link." );
        return;
    }
    
    if( this->_linked )
        return;
    
    // link
    glLinkProgram( this->_program_id );
    
    // check errors
    GLint linked;
    glGetProgramiv( this->_program_id, GL_LINK_STATUS, &linked );
    if( linked == GL_FALSE )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Shader program link error: "+GlProgram::ExtractProgramInfoLog( this->_program_id ) );
        
        glDeleteProgram( this->_program_id );
        this->_program_id = 0;
        this->_linked = false;
        
        if( this->_vertex_id )
            glDeleteShader( this->_vertex_id ),
            this->_vertex_id = 0;
            
        if( this->_fragment_id )
            glDeleteShader( this->_fragment_id ),
            this->_fragment_id = 0;
    }
    
    // clean shaders
    if( this->_vertex_id )
        glDetachShader( this->_program_id, this->_vertex_id ),
        glDeleteShader( this->_vertex_id ),
        this->_vertex_id = 0;
    
    if( this->_fragment_id )
        glDetachShader( this->_program_id, this->_fragment_id ),
        glDeleteShader( this->_fragment_id ),
        this->_fragment_id = 0;
    
    // set variables
    this->_linked = true;
    
    //
    GlError_Check( logger, SRC_INFO );
}

void GlProgram::DestroyProgram( ClientMarker const * logger )
{
    if( !this->_program_id )
        return;
    
    if( this->_vertex_id )
        glDetachShader( this->_program_id, this->_vertex_id ),
        glDeleteShader( this->_vertex_id ),
        this->_vertex_id = 0;
    
    if( this->_fragment_id )
        glDetachShader( this->_program_id, this->_fragment_id ),
        glDeleteShader( this->_fragment_id ),
        this->_fragment_id = 0;
    
    glDeleteProgram( this->_program_id );
    this->_program_id = 0;
    this->_linked = false;
    
    //
    GlError_Check( logger, SRC_INFO );
}

void GlProgram::DropProgram( ClientMarker const * logger )
{
    this->_vertex_id = 0;
    this->_fragment_id = 0;
    this->_program_id = 0;
    this->_linked = false;
}

std::string GlProgram::ExtractShaderInfoLog( GLuint shader )
{
    GLint msg_len;
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &msg_len );
    GLchar * msg = new GLchar[ msg_len ];
    glGetShaderInfoLog( shader, msg_len, nullptr, msg );
    std::string message( msg, msg_len );
    delete [] msg;
    return message;
}

std::string GlProgram::ExtractProgramInfoLog( GLuint program )
{
    GLint msg_len;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &msg_len );
    GLchar * msg = new GLchar[ msg_len ];
    glGetProgramInfoLog( program, msg_len, nullptr, msg );
    std::string message( msg );
    delete [] msg;
    return message;
}

GLuint GlProgram::program_id() const
{
    if( this->_program_id && this->_linked )
        return this->_program_id;
    else
        return 0;
}

GLint GlProgram::GetUniformLocation( const char * name ) const
{
    auto result = glGetUniformLocation( this->_program_id, name );
    //GlError_Check( wt, SRC_INFO );
    return result;
}

}
