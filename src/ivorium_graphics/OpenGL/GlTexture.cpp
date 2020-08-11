#include "GlTexture.hpp"
#include "GlError.hpp"
#include "RenderTarget.hpp"

#include <ivorium_core/ivorium_core.hpp>

#include <cmath>
#include <algorithm>

#include <ivorium_config.hpp>

namespace iv
{

const StringIO< PixelFormat >::ValuesType StringIO< PixelFormat >::Values = 
{
    { PixelFormat::RGBA, "RGBA" },
    { PixelFormat::BGRA, "BGRA" }
};

const StringIO< ColorSpace >::ValuesType StringIO< ColorSpace >::Values = 
{
    { ColorSpace::sRGB, "sRGB" },
    { ColorSpace::Linear, "Linear" }
};

GlTexture::GlTexture() :
    _texture_id( 0 ),
    _size( 0, 0 ),
    _mipmaps( false ),
    _format( PixelFormat::RGBA )
{
}

int2 GlTexture::size() const
{
    return this->_size;
}

GLuint GlTexture::texture_id() const
{
    return this->_texture_id;
}

bool GlTexture::allocated() const
{
    return this->_texture_id;
}

void GlTexture::CreateTexture( Context const * logger, RenderTarget * target, int2 size, GlMinFiltering min, GlMagFiltering mag, bool repeat, PixelFormat storage_format, ColorSpace color_space )
{
    if( this->_texture_id )
        this->DestroyTexture( logger, target );
    
    //
    this->_size = size;
    this->_format = storage_format;
    
    // compute mipmap requirements
    GLsizei levels = 1;
    this->_mipmaps = min == GlMinFiltering::NearestMipmapNearest || min == GlMinFiltering::LinearMipmapNearest || min == GlMinFiltering::NearestMipmapLinear || min == GlMinFiltering::LinearMipmapLinear;
    if( this->_mipmaps )
        levels = 1 + floor( log2( std::max( size.x, size.y ) ) );
    
    // generate texture id
    glGenTextures( 1, &this->_texture_id );
    
    // bind texture
    target->bind_texture( 0, this->_texture_id );
    
    // allocate storage
    #if IV_GLPLATFORM_GLFW
        this->_color_space = ColorSpace::Linear;
        if( color_space == ColorSpace::sRGB )
        {
            if( storage_format == PixelFormat::RGBA )
                glTexStorage2D( GL_TEXTURE_2D, 1, GL_SRGB8_ALPHA8, size.x, size.y );
            else if( storage_format == PixelFormat::BGRA )
                glTexStorage2D( GL_TEXTURE_2D, 1, GL_SRGB8_ALPHA8, size.x, size.y );
            else
                logger->log( SRC_INFO, iv::Defs::Log::Warning, "Pixel storage does not support storage format ", storage_format, " in sRGB space." );
        }
        else if( color_space == ColorSpace::Linear )
        {
            if( storage_format == PixelFormat::RGBA )
                glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA8, size.x, size.y );
            else if( storage_format == PixelFormat::BGRA )
                glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA8, size.x, size.y );
            else
                logger->log( SRC_INFO, iv::Defs::Log::Warning, "Pixel storage does not support storage format ", storage_format, " in linear space." );
        }
        else
        {
            logger->log( SRC_INFO, iv::Defs::Log::Warning, "Pixel storage does not support color space ", color_space, "." );
        }
    #elif IV_GLPLATFORM_GLFM
        
        this->_color_space = color_space;
        if( storage_format == PixelFormat::RGBA )
            glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA8, size.x, size.y );
        else if( storage_format == PixelFormat::BGRA )
            glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA8, size.x, size.y );
        else
            logger->log( SRC_INFO, iv::Defs::Log::Warning, "Pixel storage does not support storage format ", storage_format, " in linear space." );
        
    #else
        #error "Unimplemented colorspace handling for this GL platform."
    #endif
    
    // parameter - repeat
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE );
    
    // parameter - filtering
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)mag );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)min );
    
    //
    GlError_Check( logger, SRC_INFO );
}

void GlTexture::DestroyTexture( Context const * logger, RenderTarget * target )
{
    if( this->_texture_id )
    {
        glDeleteTextures( 1, &this->_texture_id );
        GlError_Check( logger, SRC_INFO );
        this->_texture_id = 0;
        this->_size = int2( 0, 0 );
        this->_mipmaps = false;
    }
}

void GlTexture::DropTexture( Context const * logger, RenderTarget * target )
{
    this->_texture_id = 0;
    this->_size = int2( 0, 0 );
    this->_mipmaps = false;
}

void GlTexture::LoadData( Context const * logger, RenderTarget * target, uint8_t * values, size_t values_size, PixelFormat data_format )
{
    // format to GL format
    GLuint gl_format;
    int bpp;
    if( data_format == PixelFormat::RGBA )
        gl_format = GL_RGBA,
        bpp = 4;
#if IV_GLPLATFORM_GLFW
    else if( data_format == PixelFormat::BGRA )
        gl_format = GL_BGRA,
        bpp = 4;
#endif
    else
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Unsupported pixel format ", data_format," when loading texture data." );
        return;
    }
    
    
    if( !this->_texture_id )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Can not load pixel data. Texture is not allocated." );
        return;
    }
    
    if( values_size != this->_size.x * this->_size.y * bpp )
    {
        logger->log( SRC_INFO, Defs::Log::Warning, "Wrong number of color data values in pixel array." );
        return;
    }
    
    // check if formats are the same
    if( data_format != this->_format )
        logger->log( SRC_INFO, Defs::Log::Performance, "Loading data to texture with different PixelFormat (runtime transformation is needed)." );
    
    // bind texture
    if( target )
        target->bind_texture( 0, this->_texture_id );
    else
        glBindTexture( GL_TEXTURE_2D, this->_texture_id );
    
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, this->_size.x, this->_size.y, gl_format, GL_UNSIGNED_BYTE, values );
    
    #warning "Disabled mimpams because it possibly causes crashes on Android."
    //if( this->_mipmaps )
    //  glGenerateMipmap( GL_TEXTURE_2D );
    
    GlError_Check( logger, SRC_INFO );
}

ColorSpace GlTexture::gpu_color_space() const
{
    return this->_color_space;
}

}
