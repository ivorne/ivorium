#include "RenderTarget.hpp"

#include <ivorium_config.hpp>
#include "gl.h"

namespace iv
{

RenderTarget::Geometry::Geometry() :
    size( 0, 0 ),
    density( 1.0 )
{
}

RenderTarget::Geometry::Geometry( float2 size, float density ) :
    size( size ),
    density( density )
{
}

RenderTarget::RenderTarget() :
    _geometry(),
    _texture_unit( 0 ),
    _texture_ids(),
    _program_id( 0 )
{
    this->_texture_ids.fill( 0 );
}

void RenderTarget::frame_setup()
{
    // depth buffer
    this->ClearDepthBuffer();
    this->DepthBufferMode( GL_ALWAYS, false );
        
    // state control
    glActiveTexture( GL_TEXTURE0 );
    this->_texture_unit = 0;
    this->_texture_ids.fill( 0 );
    this->_program_id = 0;
}

void RenderTarget::frame_close()
{
}

void RenderTarget::set_geometry( Geometry geometry )
{
    this->_geometry = geometry;
}

RenderTarget::Geometry RenderTarget::geometry() const
{
    return this->_geometry;
}

void RenderTarget::bind_texture( int texture_unit, GLuint texture_id )
{
    if( texture_unit >= MaxTexturingUnits )
        return;
    
    if( this->_texture_unit != texture_unit )
    {
        this->_texture_unit = texture_unit;
        glActiveTexture(  GL_TEXTURE0 + texture_unit );
    }
    
    if( this->_texture_ids[ texture_unit ] != texture_id )
    {
        this->_texture_ids[ texture_unit ] = texture_id;
        glBindTexture( GL_TEXTURE_2D, texture_id );
    }
}

void RenderTarget::bind_shader( GLuint program_id )
{
    if( this->_program_id != program_id )
        this->_program_id = program_id,
        glUseProgram( program_id );
}

void RenderTarget::DepthBufferClear( float value )
{
    #if IV_GLPLATFORM_GLFW
        glClearDepth( value );
    #elif IV_GLPLATFORM_GLFM
        glClearDepthf( value );
    #else
        #error "Unknown OpenGL platform."
    #endif
    glClear( GL_DEPTH_BUFFER_BIT );
}

void RenderTarget::DepthBufferMode( GLenum test_condition, bool write_enabled )
{
    if( test_condition == GL_ALWAYS && !write_enabled )
    {
        glDisable( GL_DEPTH_TEST );
    }
    else
    {
        glEnable( GL_DEPTH_TEST );
        
        glDepthFunc( test_condition );
        
        if( write_enabled )
            glDepthMask( GL_TRUE );
        else
            glDepthMask( GL_FALSE );
    }
}

void RenderTarget::Blending( bool enabled )
{
    if( enabled )
        glEnable( GL_BLEND );
    else
        glDisable( GL_BLEND );
}

void RenderTarget::ClearDepthBuffer()
{
    glClear( GL_DEPTH_BUFFER_BIT );
}

}
