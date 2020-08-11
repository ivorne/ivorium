#pragma once

#include <ivorium_core/ivorium_core.hpp>
#include <ivorium_config.hpp>
#include "gl.h"

namespace iv
{

struct GlMeshData
{
    std::vector< float > positions;
    std::vector< float > texcoords;
    std::vector< GLuint > indices;
    GLenum draw_mode;
};

class GlMesh
{
public:
    static const constexpr GLuint PrimitiveRestart = (GLuint)-1;

    static const constexpr GLuint AttributeLoc_Position = 0;    // 3 floats
    static const constexpr GLuint AttributeLoc_Texcoord = 1;    // 2 floats
    
public:
                                GlMesh();
    
    //------------------------- loading ----------------------------
    void                        CreateMesh( Context const * logger );
    
    void                        Load_Positions( Context const * logger, float const * data, size_t length );        ///< Uses 3 floats per vertex (XYZ). \p length Number of floats.
    void                        Load_TexCoords( Context const * logger, float const * data, size_t length );        ///< Uses 2 floats per vertex (UV). \p length Number of floats.
    void                        Load_Indices( Context const * logger, GLuint const * data, size_t length, GLenum draw_mode );       ///< Uses 1 unsigned per index (index id). Use value GLuint( -1 ) for primitive restart. \p length numbef or indices.
    
    void                        Load_All( Context const * logger, GlMeshData const & data );                        ///< Can be used instead of Load_* methods.
    
    void                        DestroyMesh( Context const * logger );
    void                        DropMesh( Context const * logger );
    
    //------------------------- usage ------------------------------
    /**
        This just enables VAO and calls glDrawElements.
        Shader program with uniforms and textures is expected to be enabled when this is called.
    */
    void                        DrawElements( Context const * logger ) const;
    
    size_t                      indices_cnt() const;
    
private:
    GLuint _vao;
    GLuint _position_buffer;
    GLuint _texcoord_buffer;
    GLuint _indices_buffer;
    size_t _indices_cnt;
    GLenum _draw_mode;
    
#if IV_ENABLE_TRANSFORM_FEEDBACK
    size_t _tf_triangles_cnt;
    GLuint _tf_buffer;
    std::vector< float > _tf_data;
#endif
};

}
