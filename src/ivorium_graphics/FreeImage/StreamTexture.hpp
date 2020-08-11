#pragma once

#include <ivorium_graphics/ivorium_graphics.hpp>
#include <unordered_map>

namespace iv
{

/**
    Reads all information about texture files from /metadata.json resource file.
    It is resource, so it gets loaded only one and can then be used by all textures.
*/
class StreamTexture_Subprovider : private StreamResourceSubprovider
{
public:
using StreamResourceSubprovider::instance;
ClientMarker cm;
    
                                    StreamTexture_Subprovider( Instance * inst, StreamResourceProvider const * );
    void                            each_resource( std::function< void( ResourcePath const & ) > const & ) const;
    bool                            has_resource( ResourcePath const & ) const;
    
    // used by StreamTexture
    Texture::Metadata const &       get( ResourcePath const & path ) const;
    
private:
    //----------------------------- StreamMetadata ----------------
    virtual void                    Resource( std::string const & resource_class, ResourcePath path ) override;
    
private:
    std::unordered_map< ResourcePath, Texture::Metadata > _textures;
    Texture::Metadata _empty;
};

/**
*/
class StreamTexture : public Texture, private StreamResource, protected GlListener, protected DelayedLoad
{
public:
using Texture::instance;
ClientMarker cm;
                                            /**
                                                Disable delayed_loading in resources used in loading screens (we don't want to load loading screen resource during loading).
                                            */
                                            StreamTexture( Instance * inst, StreamResourceProvider const * provider, StreamTexture_Subprovider const * subprovider, ResourcePath const & path );
    virtual                                 ~StreamTexture();
    
protected:
    //------------------------------------- GlListener ---------------------
    virtual void                            GlEnable() override;
    virtual void                            GlDisable() override;
    virtual void                            GlDrop() override;
    
    //------------------------------------- DelayedLoad ---------------------
    virtual void                            Load() override;
    
    GlMinFiltering                          min_filtering();
    GlMagFiltering                          mag_filtering();
};

}
