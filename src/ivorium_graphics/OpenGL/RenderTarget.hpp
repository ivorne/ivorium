#pragma once

#include "gl.h"
#include <ivorium_core/ivorium_core.hpp>
#include <array>

namespace iv
{

class RenderTarget
{
public:
    /**
        This is used so that I can handle more than one texturing unit.
        To realy reliably use more than 2 (guaranteed for GLES), I should read that value from GL, make this dynamic (dynamically allocated arrays) and do some verification (error message on fail).
    */
    static const constexpr int MaxTexturingUnits = 2;

    struct Geometry
    {
        /**
            Window size in physical pixels.
            Group of [ density * density ] physical pixels make up one logical pixel.
        */
        int2 size;
        
        /**
            Pixel density.
            Width and height of a logical pixel in physical pixels.
        */
        float  density;
        
        Geometry();
        Geometry( float2 size, float density );
    };

public:
                    RenderTarget();
    virtual         ~RenderTarget(){}
    
    // depth buffer
    void            DepthBufferClear( float value );
    void            DepthBufferMode( GLenum test_condition, bool write_enabled );       ///< \p test_condition GL_ALWAYS, GL_LESS, GL_GREATER, ... (is forwarded to glDepthFunc).
    
    // blending
    void            Blending( bool enabled );
    
    // geometry
    Geometry        geometry() const;
    
    // state control
    void            bind_texture( int texture_unit, GLuint texture_id );
    void            bind_shader( GLuint program_id );
    
protected:
    /**
        Called before each render, when gl context is already available.
        This will reset the state of opengl context to default.
        It will also reset state of RenderTarget.
    */
    void            frame_setup();
    
    /**
        Called after each render.
        This clears bound textures and such.
        Might get used for safety - so that bound gl texture from render does not get modified by invalid code outside of render.
    */
    void            frame_close();
    
    /**
        Geometry values are used during rendering by renderer components.
    */
    void            set_geometry( Geometry geometry );
    
private:
    void            ClearDepthBuffer();
    
private:
    Geometry                                    _geometry;
    
    // state cache
    int                                         _texture_unit;      ///< Unit set with glActiveTexture.
    std::array< GLuint, MaxTexturingUnits >     _texture_ids;       ///< Texture bound to texturing unit with glBindTexture. Mode is always GL_TEXTURE_2D.
    GLuint                                      _program_id;        ///< Shader program set with glUseProgram.
};

}
