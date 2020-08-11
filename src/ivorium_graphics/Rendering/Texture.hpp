#pragma once

#include "GlSystem.hpp"
#include "../OpenGL/GlTexture.hpp"

#include <ivorium_systems/ivorium_systems.hpp>
#include <ivorium_core/ivorium_core.hpp>

#include <optional>

namespace iv
{

//---------------------------------------------------------------------------------------
/**
*/
class Texture
{
public:
    enum class Filtering
    {
        Nearest,
        SmoothMsdf,
        Smooth2D,
        Smooth3D
    };
    
    struct Metadata
    {
        int2                    size;               ///< Physical size. It is better to take size from here, rather than from texture, because this does not need to read the file iself.
        float                   density;            ///< Number of physical pixels in a virtual pixel.
        Filtering               filtering;
        PixelFormat             format;
        ColorSpace              color_space;
        float                   msdf_pixelRange;    ///< Only used when rendering this as msdf texture.
        
        bool                    hitmap;             ///< If the texture will be pixel-precisely hit-tested. If this is disabled, then pixel-precise hittest will return always true.
        bool                    instant_loading;    ///< Enabled for resources that are used on loading screens - they load instantly, so they don't queue in loading screen queues.
        
        Metadata();
    };
    
public:
ClientMarker cm;

                            Texture( Instance * inst );
    Instance *              instance() const;
    
    Metadata const &        metadata() const;
    GlTexture const *       gl_texture() const;                 ///< Might be loaded with delay. Is unloaded when gpu is off. Is never nullptr.
    bool                    hittest( int x, int y ) const;      ///< Might be loaded with delay.

protected:
    Metadata                _metadata;
    GlTexture               _gl_texture;
    std::vector< bool >     _hitmap;

private:
    Instance * inst;
};

//---------------------------------------------------------------------------------------
/**
*/
class Texture_Resource : public SingularResource< Texture >
{
public:
ClientMarker cm;
    Texture_Resource( Instance * inst, ResourcePath const & path ) :
        SingularResource< Texture >( inst, path ),
        cm( inst, this, "Texture_Resource" )
    {
        this->cm.inherits( this->SingularResource< Texture >::cm );
    }
};

//--------------------------------- StingIO ---------------------------------------------
template<>
struct StringIO< Texture::Filtering > : public StringIO_Table< Texture::Filtering >
{
    static const ValuesType Values;
};

}
