#include "StreamTexture.hpp"
#include "ImageLoader.hpp"

namespace iv
{

//======================== TextureMetadata ==============================
StreamTexture_Subprovider::StreamTexture_Subprovider( Instance * inst, StreamResourceProvider const * provider ) :
    StreamResourceSubprovider( inst ),
    cm( inst, this, "StreamTexture_Subprovider" )
{
    this->cm.inherits( this->StreamResourceSubprovider::cm );
    
    provider->with_metadata_stream(
        [&]( std::istream & metadata_file )
        {
            this->Parse_EachResource( metadata_file );
        }
    );
}

void StreamTexture_Subprovider::Resource( std::string const & resource_class, ResourcePath path )
{
    if( resource_class == "texture" )
    {
        Texture::Metadata vals;
        
        this->Parse_EachParameter(
            [ & ]( std::string const & param, std::string const & value )
            {
                if( param == "width" )
                    vals.size.x = atoi( value.c_str() );
                else if( param == "height" )
                    vals.size.y = atoi( value.c_str() );
                else if( param == "density" )
                    vals.density = atof( value.c_str() );
                else if( param == "hitmap" )
                    vals.hitmap = value == "True";
                else if( param == "instant_loading" )
                    vals.instant_loading = value == "True";
                else if( param == "filtering" )
                {
                    if( value == "Nearest" )
                        vals.filtering = Texture::Filtering::Nearest;
                    else if( value == "SmoothMsdf" )
                        vals.filtering = Texture::Filtering::SmoothMsdf;
                    else if( value == "Smooth2D" )
                        vals.filtering = Texture::Filtering::Smooth2D;
                    else if( value == "Smooth3D" )
                        vals.filtering = Texture::Filtering::Smooth3D;
                    else
                    {
                        this->cm.warning( SRC_INFO, "Unrecognized 'filtering' value '",value,"'." );
                        vals.filtering = Texture::Filtering::Nearest;
                    }
                }
                else if( param == "color_space" )
                {
                    if( value == "linear" )
                        vals.color_space = ColorSpace::Linear;
                    else if( value == "sRGB" )
                        vals.color_space = ColorSpace::sRGB;
                    else
                    {
                        this->cm.warning( SRC_INFO, "Unrecognized 'color_space' value '",value,"'." );
                        vals.color_space = ColorSpace::sRGB;
                    }
                }
                else if( param == "msdf_pixelRange" )
                    vals.msdf_pixelRange = atof( value.c_str() );
                else if( param == "pixel_format" )
                {
                    if( value == "RGBA" )
                        vals.format = PixelFormat::RGBA;
                    else
                    {
                        this->cm.warning( SRC_INFO, "Unrecognized 'pixel_format' value '",value,"'." );
                        vals.format = PixelFormat::RGBA;
                    }
                }
                else
                {
                    this->cm.warning( SRC_INFO, "Unrecognized metadata parameter '",param,"'." );
                }
            }
        );
        
        this->_textures[ path ] = vals;
    }
    else
    {
        this->Parse_EachParameter();
    }
}

void StreamTexture_Subprovider::each_resource( std::function< void( ResourcePath const & ) > const & f ) const
{
    for( auto & [ path, metadata ] : this->_textures )
        f( path );
}

bool StreamTexture_Subprovider::has_resource( ResourcePath const & path ) const
{
    return this->_textures.count( path );
}

Texture::Metadata const & StreamTexture_Subprovider::get( ResourcePath const & path ) const
{
    auto it = this->_textures.find( path );
    if( it != this->_textures.end() )
    {
        return it->second;
    }
    else
    {
        this->cm.warning( SRC_INFO, "ImageMetadata not found for resource '", path.string(), "'." );
        return this->_empty;
    }
}

//============================== StreamTexture ================================================================
StreamTexture::StreamTexture( Instance * inst, StreamResourceProvider const * provider, StreamTexture_Subprovider const * subprovider, ResourcePath const & path ) :
    Texture( inst ),
    StreamResource( inst, provider, path ),
    GlListener( inst ),
    DelayedLoad( inst ),
    cm( inst, this, "StreamTexture" )
{
    // mark
    this->cm.inherits( this->Texture::cm, this->StreamResource::cm, this->DelayedLoad::cm );
    
    // load metadata
    this->_metadata = subprovider->get( path );
    
    // gl enable
    this->cm.log( SRC_INFO, ::iv::Defs::Log::TextureState, "Texture created." );
    if( this->GlIsEnabled() )
        this->GlEnable();
    else
        this->cm.log( SRC_INFO, ::iv::Defs::Log::TextureState, "Not loading - OpenGL not enabled yet." );
}

StreamTexture::~StreamTexture()
{
    GlInfo gl_info( this->instance() );
    this->_gl_texture.DestroyTexture( &this->cm, gl_info.render_target() );
}

void StreamTexture::GlEnable()
{
    if( this->_metadata.instant_loading )
    {
        this->cm.log( SRC_INFO, ::iv::Defs::Log::TextureState, "Loading to GPU (instant loading is enabled for this texture)." );
        this->Load();
    }
    else
    {
        auto complexity_bytes = this->_metadata.size.x * this->_metadata.size.y * 4;
        if( this->_metadata.hitmap )
            complexity_bytes *= 2;
        this->cm.log( SRC_INFO, ::iv::Defs::Log::TextureState, "Queuing delayed load (instant loading is disabled for this texture)." );
        this->RequestDelayedLoad( complexity_bytes );
    }
}

void StreamTexture::GlDisable()
{
    this->cm.log( SRC_INFO, ::iv::Defs::Log::TextureState, "Unloading texture - OpenGl is disabled." );
    GlInfo gl_info( this->instance() );
    this->_gl_texture.DestroyTexture( &this->cm, gl_info.render_target() );
}

void StreamTexture::GlDrop()
{
    this->cm.log( SRC_INFO, ::iv::Defs::Log::TextureState, "Dropping texture - OpenGl is dropped." );
    GlInfo gl_info( this->instance() );
    this->_gl_texture.DropTexture( &this->cm, gl_info.render_target() );
}

void StreamTexture::Load()
{
    if( !this->GlIsEnabled() )
    {
        this->cm.log( SRC_INFO, ::iv::Defs::Log::TextureState, "Texture load skipped - OpenGL is disabled." );
        return;
    }
    
    this->cm.log( SRC_INFO, ::iv::Defs::Log::TextureState, "Calling ImageLoader." );
    ImageLoader_Load( this, this->_metadata.format,
        [&]( ImageLoader_Metadata const & metadata, uint8_t * bytes, size_t bytes_count )
        {
            // load texture to GPU
            this->cm.log( SRC_INFO, ::iv::Defs::Log::TextureState, "Loading texture to GPU." );
            GlInfo gl_info( this->instance() );
            this->_gl_texture.CreateTexture( &this->cm, gl_info.render_target(), metadata.size, this->min_filtering(), this->mag_filtering(), false, metadata.pixel_format, this->_metadata.color_space );
            this->_gl_texture.LoadData( &this->cm, gl_info.render_target(), bytes, bytes_count, metadata.pixel_format );
            
            // load hitmap
            if( this->_metadata.hitmap )
                if( this->_hitmap.empty() )
                {
                    this->cm.log( SRC_INFO, ::iv::Defs::Log::TextureState, "Loading hitmap for texture." );
                    
                    // load hittest data
                    this->_hitmap.resize( metadata.size.x * metadata.size.y );
                    for( size_t x = 0; x < metadata.size.x; x++ )
                        for( size_t y = 0; y < metadata.size.y; y++ )
                        {
                            uint8_t pixel = 0;
                            if( metadata.pixel_format == PixelFormat::RGBA || metadata.pixel_format == PixelFormat::BGRA )
                                pixel = bytes[ ( y * metadata.size.x + x ) * 4 + 3 ];
                            else
                                this->cm.log( SRC_INFO, Defs::Log::Warning, "Unsupported pixel format when generating hitmap." );
                            
                            this->_hitmap[ y * metadata.size.x + x ] = pixel > 0;
                        }
                }
        }
    );
}

GlMinFiltering StreamTexture::min_filtering()
{
    if( this->_metadata.filtering == Texture::Filtering::SmoothMsdf )
        return GlMinFiltering::Linear;
    else if( this->_metadata.filtering == Texture::Filtering::Smooth2D )
        return GlMinFiltering::LinearMipmapNearest;
    else if( this->_metadata.filtering == Texture::Filtering::Smooth3D )
        return GlMinFiltering::LinearMipmapLinear;
    else
        return GlMinFiltering::NearestMipmapNearest;
}

GlMagFiltering StreamTexture::mag_filtering()
{
    if( this->_metadata.filtering == Texture::Filtering::SmoothMsdf )
        return GlMagFiltering::Linear;
    else if( this->_metadata.filtering == Texture::Filtering::Smooth2D )
        return GlMagFiltering::Linear;
    else if( this->_metadata.filtering == Texture::Filtering::Smooth3D )
        return GlMagFiltering::Linear;
    else
        return GlMagFiltering::Nearest;
}

}
