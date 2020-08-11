#include "GlMesh.hpp"
#include "../Defs.hpp"
#include "GlError.hpp"
#include <ivorium_config.hpp>

namespace iv
{

GlMesh::GlMesh() :
    _vao( 0 ),
    _position_buffer( 0 ),
    _texcoord_buffer( 0 ),
    _indices_buffer( 0 ),
    _indices_cnt( 0 )
#if IV_ENABLE_TRANSFORM_FEEDBACK
    , _tf_triangles_cnt( 0 )
    , _tf_buffer( 0 )
#endif
{
}

void GlMesh::Load_All( Context const * logger, GlMeshData const & data )
{
    this->Load_Positions( logger, data.positions.data(), data.positions.size() );
    this->Load_TexCoords( logger, data.texcoords.data(), data.texcoords.size() );
    this->Load_Indices( logger, data.indices.data(), data.indices.size(), data.draw_mode );
}

size_t GlMesh::indices_cnt() const
{
    return this->_indices_cnt;
}

void GlMesh::CreateMesh( Context const * logger )
{
    if( this->_vao )
        this->DestroyMesh( logger );
    
    glGenVertexArrays( 1, &this->_vao );
}

void GlMesh::Load_Positions( Context const * logger, float const * data, size_t length )
{
    if( !this->_vao )
        this->CreateMesh( logger );
    
    if( !this->_position_buffer )
    {
        // create buffer
        glGenBuffers( 1, &this->_position_buffer );
        
        // map it to VAO
        glBindBuffer( GL_ARRAY_BUFFER, this->_position_buffer );
        glBindVertexArray( this->_vao );
        glVertexAttribPointer( AttributeLoc_Position, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
        glEnableVertexAttribArray( AttributeLoc_Position );
        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
    
    // load data
    glBindBuffer( GL_ARRAY_BUFFER, this->_position_buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * length, data, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
    GlError_Check( logger, SRC_INFO );
}

void GlMesh::Load_TexCoords( Context const * logger, float const * data, size_t length )
{
    if( !this->_vao )
        this->CreateMesh( logger );
    
    if( !this->_texcoord_buffer )
    {
        // create buffer
        glGenBuffers( 1, &this->_texcoord_buffer );
        
        // map it to VAO
        glBindBuffer( GL_ARRAY_BUFFER, this->_texcoord_buffer );
        glBindVertexArray( this->_vao );
        glVertexAttribPointer( AttributeLoc_Texcoord, 2, GL_FLOAT, GL_FALSE, 0, nullptr );
        glEnableVertexAttribArray( AttributeLoc_Texcoord );
        glBindVertexArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
    
    // load data
    glBindBuffer( GL_ARRAY_BUFFER, this->_texcoord_buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * length, data, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
    GlError_Check( logger, SRC_INFO );
}

void GlMesh::Load_Indices( Context const * logger, GLuint const * data, size_t length, GLenum draw_mode )
{
    if( !this->_vao )
        this->CreateMesh( logger );
    
    this->_draw_mode = draw_mode;
    
#if IV_ENABLE_TRANSFORM_FEEDBACK
    // count triangles
    size_t triangles = 0;
    if( draw_mode == GL_TRIANGLE_STRIP )
    {
        size_t active = 0;
        for( size_t i = 0; i < length; i++ )
        {
            if( data[ i ] == PrimitiveRestart )
                active = 0;
            else
                active += 1;
                
            if( active >= 3 )
                triangles += 1;
        }
    }
    else
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Unimplemented draw mode ", draw_mode, "for transform feedback. Transform feedback inactive." );
    }
    this->_tf_triangles_cnt = triangles;
    
#endif
    
    //
    if( !this->_indices_buffer )
    {
        // create buffer
        glGenBuffers( 1, &this->_indices_buffer );
        
        // map it to VAO
        glBindVertexArray( this->_vao );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->_indices_buffer ); //< NOTE - It seems that this does not bind element buffer to the VAO for some reason
        glBindVertexArray( 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }
    
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->_indices_buffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLuint ) * length, data, GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    
    this->_indices_cnt = length;
    
    #if IV_ENABLE_TRANSFORM_FEEDBACK
    if( this->_tf_buffer )
        glDeleteBuffers( 1, &this->_tf_buffer );
    
    glGenBuffers( 1, &this->_tf_buffer );
    glBindBuffer( GL_TRANSFORM_FEEDBACK_BUFFER, this->_tf_buffer );
    glBufferData( GL_TRANSFORM_FEEDBACK_BUFFER, sizeof( float ) * this->_tf_triangles_cnt * 3 * 4, nullptr, GL_DYNAMIC_READ );
    glBindBuffer( GL_TRANSFORM_FEEDBACK_BUFFER, 0 );
    
    this->_tf_data.resize( this->_tf_triangles_cnt * 3 * 4 );
    #endif
    
    GlError_Check( logger, SRC_INFO );
}

void GlMesh::DestroyMesh( Context const * logger )
{
    if( this->_vao )
    {
        glDeleteVertexArrays( 1, &this->_vao );
        this->_vao = 0;
    }
    
    if( this->_position_buffer )
    {
        glDeleteBuffers( 1, &this->_position_buffer );
        this->_position_buffer = 0;
    }
    
    if( this->_texcoord_buffer )
    {
        glDeleteBuffers( 1, &this->_texcoord_buffer );
        this->_texcoord_buffer = 0;
    }
    
    if( this->_indices_buffer )
    {
        glDeleteBuffers( 1, &this->_indices_buffer );
        this->_indices_buffer = 0;
    }
    
    #if IV_ENABLE_TRANSFORM_FEEDBACK
    if( this->_tf_buffer )
    {
        glDeleteBuffers( 1, &this->_tf_buffer );
        this->_tf_buffer = 0;
    }
    #endif
    
    this->_indices_cnt = 0;
    this->_draw_mode = GL_TRIANGLE_STRIP;
    
    GlError_Check( logger, SRC_INFO );
}

void GlMesh::DropMesh( Context const * logger )
{
    this->_vao = 0;
    this->_position_buffer = 0;
    this->_texcoord_buffer = 0;
    this->_indices_buffer = 0;
    this->_indices_cnt = 0;
    #if IV_ENABLE_TRANSFORM_FEEDBACK
    this->_tf_buffer = 0;
    #endif
    this->_draw_mode = GL_TRIANGLE_STRIP;
}

void GlMesh::DrawElements( Context const * logger ) const
{
    if( !this->_vao )
        return;
    
    #if IV_ENABLE_TRANSFORM_FEEDBACK
    bool tf_enabled = logger->log_enabled( Defs::Log::TransformFeedback );
    if( tf_enabled )
    {
        glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, this->_tf_buffer );
        glBeginTransformFeedback( GL_TRIANGLES );
    }
    #endif
    
    glBindVertexArray( this->_vao );
    glDrawElements( this->_draw_mode, this->_indices_cnt, GL_UNSIGNED_INT, nullptr );
    glBindVertexArray( 0 );
    
    #if IV_ENABLE_TRANSFORM_FEEDBACK
    if( tf_enabled )
    {
        // end TF
        glEndTransformFeedback();
        
        // bind
        glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0 );
        glBindBuffer( GL_TRANSFORM_FEEDBACK_BUFFER, this->_tf_buffer );
        
        // read buffer
        size_t tf_buffer_length = sizeof( float ) * this->_tf_triangles_cnt * 3 * 4;
        #if IV_GLPLATFORM_GLFW
        {
            glGetBufferSubData( GL_TRANSFORM_FEEDBACK_BUFFER, 0, tf_buffer_length, (void*)this->_tf_data.data() );
        }
        #elif IV_GLPLATFORM_GLFM
        {
            void * buff = glMapBufferRange( GL_TRANSFORM_FEEDBACK_BUFFER, 0, tf_buffer_length, GL_MAP_READ_BIT );
            if( buff )
                memcpy( (void*)this->_tf_data.data(), buff, tf_buffer_length );
            else
                logger->log_frame( Defs::Log::TransformFeedback, "Failed to retrieve transform feedback (glMapBufferRange returns nullptr)." );
            
            glUnmapBuffer( GL_TRANSFORM_FEEDBACK_BUFFER );
            GlError_Check( logger, SRC_INFO );
        }
        #else
            #error "Unknown OpenGL platform."
        #endif
        
        // unbind
        glBindBuffer( GL_TRANSFORM_FEEDBACK_BUFFER, 0 );
        
        for( size_t i = 0; i < this->_tf_triangles_cnt; i++ )
        {
            logger->log( SRC_INFO, Defs::Log::TransformFeedback,
                "Triangle", Context::Endl(),
                Context::Begin(),
                    " (", this->_tf_data[ i*12 + 0 ], ", ", this->_tf_data[ i*12 + 1 ], ", ", this->_tf_data[ i*12 + 2 ], ", ", this->_tf_data[ i*12 + 3 ], ")", Context::Endl(),
                    " (", this->_tf_data[ i*12 + 4 ], ", ", this->_tf_data[ i*12 + 5 ], ", ", this->_tf_data[ i*12 + 6 ], ", ", this->_tf_data[ i*12 + 7 ], ")", Context::Endl(),
                    " (", this->_tf_data[ i*12 + 8 ], ", ", this->_tf_data[ i*12 + 9 ], ", ", this->_tf_data[ i*12 + 10 ], ", ", this->_tf_data[ i*12 + 11 ], ")",
                Context::End()
            );
        }
    }
    #endif
}

}
