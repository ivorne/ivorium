#pragma once

#include <ivorium_core/ivorium_core.hpp>
#include "gl.h"

namespace iv
{

class RenderTarget;

enum class GlMagFiltering
{
    Nearest         = GL_NEAREST,
    Linear          = GL_LINEAR,
};

enum class GlMinFiltering
{
    Nearest                 = GL_NEAREST,
    Linear                  = GL_LINEAR,
    NearestMipmapNearest    = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapNearest     = GL_LINEAR_MIPMAP_NEAREST,
    NearestMipmapLinear     = GL_NEAREST_MIPMAP_LINEAR,
    LinearMipmapLinear      = GL_LINEAR_MIPMAP_LINEAR,
};

enum class PixelFormat
{
    RGBA,       // 8R 8G 8B 8A, in sRGB space
    BGRA,       // 8B 8G 8R 8A, in sRGB space
};

enum class ColorSpace
{
    sRGB,
    Linear,
};

/**
    Uses basic self-repairing state control inside create_texture, delete_texture, drop_texture loop.
    Calling create_texture second time will automatically delete previous texture.
    Constructors and destructors don't create or delete texture, so this should be done by a wrapper class, otherwise, it's a leak.
    
    RenderTarget should be set to valid value when calling this from draw method.
*/
class GlTexture
{
public:
                                GlTexture();
    
    //------------------------- loading ------------------------------
    /**
        Creates (or recreated) texture in opengl context.
        Texture is stored using given PixelFormat.
    */
    void                        CreateTexture( Context const * logger, RenderTarget * target, int2 size, GlMinFiltering, GlMagFiltering, bool repeat, PixelFormat storage_format, ColorSpace color_space );
    
    /**
        The data_format can be different than the format used in CreateTexture - data will be transformed, but performance warning will be printed to log.
    */
    void                        LoadData( Context const * logger, RenderTarget * target, uint8_t * values, size_t values_size, PixelFormat data_format );
    
    /**
    */
    void                        DestroyTexture( Context const * logger, RenderTarget * target );
    
    /**
        Disassocieates GLuint texture_id from this instance without destroying it.
    */
    void                        DropTexture( Context const * logger, RenderTarget * target );
    
    //------------------------- getters --------------------------------
    bool                        allocated() const;
    int2                        size() const;
    GLuint                      texture_id() const;
    ColorSpace                  gpu_color_space() const;    ///< Color space in shaders = in which colorspace will texels be when fetched in shader.
    
private:
    GLuint  _texture_id;
    int2    _size;
    bool    _mipmaps;
    PixelFormat _format;
    ColorSpace _color_space;
};

template<>
struct StringIO< PixelFormat > : public StringIO_Table< PixelFormat >
{
    static const ValuesType Values;
};

template<>
struct StringIO< ColorSpace > : public StringIO_Table< ColorSpace >
{
    static const ValuesType Values;
};

}
