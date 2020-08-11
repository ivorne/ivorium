#include "Texture.hpp"

#include <ivorium_core/ivorium_core.hpp>
#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

const StringIO< Texture::Filtering >::ValuesType StringIO< Texture::Filtering >::Values = 
{
    { Texture::Filtering::Nearest, "Nearest" },
    { Texture::Filtering::SmoothMsdf, "SmoothMsdf" },
    { Texture::Filtering::Smooth2D, "Smooth2D" },
    { Texture::Filtering::Smooth3D, "Smooth3D" }
};

Texture::Metadata::Metadata() :
    size( 0, 0 ),
    density( 1.0 ),
    filtering( Filtering::Nearest ),
    format( PixelFormat::RGBA ),
    color_space( ColorSpace::sRGB ),
    msdf_pixelRange( 1.0f ),
    hitmap( false ),
    instant_loading( false )
{
}

Texture::Texture( Instance * inst ) :
    cm( inst, this, "Texture" ),
    _metadata(),
    _gl_texture(),
    _hitmap(),
    inst( inst )
{
}

Instance * Texture::instance() const
{
    return this->inst;
}

Texture::Metadata const & Texture::metadata() const
{
    return this->_metadata;
}

GlTexture const * Texture::gl_texture() const
{
    return &this->_gl_texture;
}

bool Texture::hittest( int x, int y ) const
{
    int id = y * this->_metadata.size.x + x;
    if( id < 0 || id >= this->_hitmap.size() )
        return false;
    return this->_hitmap[ id ];
}

}
