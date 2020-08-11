#include "Shader.hpp"

#include <ivorium_core/ivorium_core.hpp>
#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

Shader::Shader( Instance * inst, ResourcePath const & path ) :
    cm( inst, this, "Shader", ClientMarker::Status() ),
    inst( inst ),
    _src_path( path ),
    _gl_program()
{
}

Shader::~Shader()
{
    this->_gl_program.DropProgram( &this->cm );
}

Instance * Shader::instance() const
{
    return this->inst;
}

void Shader::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Shader" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "filename", this->_src_path );
}

void Shader::LoadProgram()
{
    ResourcePath vert_path = this->_src_path + ".gl.vert";
    ResourcePath frag_path = this->_src_path + ".gl.frag";
    
    this->_gl_program.CreateProgram( &this->cm );
    
    DataStream_Resource vert_data( this->instance(), vert_path );
    vert_data->with_stream(
        [&]( std::istream & in )
        {
            this->_gl_program.Load_VertexShader( &this->cm, in );
        }
    );
    
    DataStream_Resource frag_data( this->instance(), frag_path );
    frag_data->with_stream(
        [&]( std::istream & in )
        {
            this->_gl_program.Load_FragmentShader( &this->cm, in );
        }
    );
}

void Shader::BindAttribute( GLuint location, const char * attrib_name )
{
    this->_gl_program.BindAttribute( &this->cm, location, attrib_name );
}

void Shader::PositionAttributeName( const char * name )
{
    this->_gl_program.PositionAttributeName( &this->cm, name );
}

void Shader::LinkProgram()
{
    this->_gl_program.LinkProgram( &this->cm );
}

void Shader::UnloadProgram()
{
    this->_gl_program.DestroyProgram( &this->cm );
}

void Shader::DropProgram()
{
    this->_gl_program.DropProgram( &this->cm );
}

GLint Shader::GetUniformLocation( const char * name ) const
{
    auto result = this->_gl_program.GetUniformLocation( name );
    if( result == (GLint)-1 )
        this->cm.warning( SRC_INFO, "Uniform '", name, "' not found." );
    return result;
}

GlProgram const * Shader::gl_program() const
{
    return &this->_gl_program;
}

}
