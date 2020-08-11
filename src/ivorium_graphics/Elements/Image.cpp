#include "Image.hpp"

#include "../Defs.hpp"

namespace iv
{

Image::Image( Instance * inst ) :
    Elem( inst ),
    SlotChild( this ),
    TranslucentElem( this ),
    cm( inst, this, "Image", ClientMarker::Status() ),
    attr_filename( &this->cm, "/ivorium_graphics/empty.png" ),
    shading( &this->cm ),
    texture( inst, ResourcePath() ),
    shader( inst ),
    square( inst )
{
    this->cm.inherits( this->Elem::cm, this->SlotChild::cm, this->TranslucentElem::cm );
    this->cm.owns( this->texture.cm, this->shader.cm, this->square.cm );
}

void Image::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Image" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "filename", this->attr_filename.Get() );
    
    row.Column( "fittingStage", this->shading.fittingStage );
    
    row.Column( "pixelizeStage", this->shading.pixelizeStage )
        .Hint( "size", this->shading.pixelizeSize.Get() )
        .Hint( "offset", this->shading.pixelizeOffset.Get() );
    
    row.Column( "resizeStage", this->shading.resizeStage )
        .Hint( "anchor", this->shading.resizeAnchor.Get() );
    
    row.Column( "filteringStage", this->shading.filteringStage )
        .Hint( "filteringAlphaThreshold", this->shading.filteringAlphaThreshold.Get() )
        .Hint( "filteringAlphaWidth", this->shading.filteringAlphaWidth.Get() );
    
    row.Column( "alpha", this->shading.alpha.Get() );
    
    row.Column( "colorTransform", this->shading.colorTransform.Get() );
}

void Image::first_pass_impl( ElementRenderer * er )
{
    this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "First pass." );
    
    if( this->attr_filename.dirty() )
    {
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "Refresh preferred_size (filename changed)." );
    
        er->Notify_FirstPass_Refresh( this );
        
        // read texture
        this->texture.path( this->attr_filename.Get() );
        this->attr_filename.clear_dirty();
        
        // update prefsize
        if( this->texture.get() )
        {
            int2 prefsize = this->texture->metadata().size;
            float density = this->texture->metadata().density;
            this->preferredSize.Set( float3( prefsize.x / density, prefsize.y / density, 0 ) );
        }
        else
        {
            this->preferredSize.Set( float3( 0, 0, 0 ) );
        }
    }
    
    // queue render
    if( this->shading.alpha.Get() != 0.0f )
    {
        GLuint shader_id = this->shader.get() ? this->shader->program_id() : 0;
        GLuint texture_id = this->texture.get() ? this->texture->gl_texture()->texture_id() : 0;
        
        if( shader_id && texture_id )
        {
            if( this->attr_translucent.Get() )
            {
                this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "Queue render pass (translucent)." );
                er->AddRenderable_Translucent( this, shader_id, texture_id );
            }
            else
            {
                this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "Queue render pass (solid)." );
                er->AddRenderable_Solid( this, shader_id, texture_id );
            }
        }
        else
        {
            this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "Render skipped (shader or texture are not loaded yet)." );
        }
    }
    else
    {
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "Render skipped (alpha == 0)." );
    }
}

void Image::second_pass_impl( ElementRenderer * er )
{
}

void Image::render( CameraState const & camera, std::optional< float > draw_order_depth )
{
    this->cm.log( SRC_INFO, Defs::Log::ElementRenderPass, "Render ", this->attr_filename.Get(), "." );
    
    float3 size = this->size.Get();
    
    this->shader->Render(
        this->cm,
        
        camera,
        draw_order_depth,
        
        this->modelTransform.Get(),
        this->scissor.Get(),
        
        this->attr_preblend.Get(),
        this->attr_translucent.Get(),
        
        this->square->gl_mesh(),
        float3( size.x, size.y, 0.0f ),
        float2( 1.0f, 1.0f ),
        
        this->texture->gl_texture(),
        this->texture->metadata().density,
        this->texture->metadata().msdf_pixelRange,
        
        this->shading
    );
}

float Image::texcoord_frame_transform_px( float coord_image_px, float tex_size, float image_size )
{
    float coord_tex_px;
    if( coord_image_px < image_size * 0.5 )
    {// left half
        if( coord_image_px < tex_size / 2.0f )
            coord_tex_px = coord_image_px;
        else
            coord_tex_px = tex_size / 2.0f;
    }
    else
    {// right half
        if( image_size - coord_image_px < tex_size / 2.0f )
            coord_tex_px = tex_size - ( image_size - coord_image_px );
        else
            coord_tex_px = tex_size / 2.0f;
    }
    
    return coord_tex_px;
}

bool Image::picking_test_pixel_perfect( float2 local_pos )
{
    if( !this->texture.get() )
        return false;
    
    if( !this->texture->metadata().hitmap )
        return true;
    
    float2 size( this->size.Get().x, this->size.Get().y );
    float2 texsize( this->texture->metadata().size );
    float dpi = this->texture->metadata().density;
    
    int2 texpos;
    if( this->shading.resizeStage == FlatShader::ResizeStage::Scale )
    {
        texpos = local_pos / size * texsize;
    }
    else if( this->shading.resizeStage == FlatShader::ResizeStage::Repeat )
    {
        auto resizeAnchor = this->shading.resizeAnchor.Get();
        texpos = local_pos - resizeAnchor * size + resizeAnchor * texsize / dpi;
        texpos.x = sig_mod( texpos.x, texsize.x / dpi ) * dpi;
        texpos.y = sig_mod( texpos.y, texsize.y / dpi ) * dpi;
    }
    else if( this->shading.resizeStage == FlatShader::ResizeStage::Frame )
    {
        texpos = int2( Image::texcoord_frame_transform_px( local_pos.x, texsize.x / dpi, size.x ) * dpi, Image::texcoord_frame_transform_px( local_pos.y, texsize.y / dpi, size.y ) * dpi );
    }
    else // FlatShader::ResizeStage::Fixed
    {
        auto resizeAnchor = this->shading.resizeAnchor.Get();
        texpos = ( local_pos - resizeAnchor * size + resizeAnchor * texsize / dpi ) * dpi;
    }
    
    
    this->cm.log( SRC_INFO, Defs::Log::Picking,
                            "pixel_text:", Context::Endl(),
                            "    texsize=", texsize, Context::Endl(),
                            "    texpos=", texpos );
    
    return this->texture->hittest( texpos.x, texsize.y - texpos.y - 1 );
}

Image * Image::enabled( bool val )
{
    this->Elem::enabled( val );
    return this;
}

Image * Image::preblend( float4 val )
{
    this->TranslucentElem::preblend( val );
    return this;
}

Image * Image::translucent( bool val )
{
    this->TranslucentElem::translucent( val );
    return this;
}

Image * Image::filename( ResourcePath const & val )
{
    this->attr_filename.Set( val );
    return this;
}

Image * Image::fittingStage( FlatShader::FittingStage v )
{
    this->shading.fittingStage = v;
    return this;
}

Image * Image::pixelizeStage( FlatShader::PixelizeStage v )
{
    this->shading.pixelizeStage = v;
    return this;
}

Image * Image::pixelizeSize( float2 v )
{
    this->shading.pixelizeSize.Set( v );
    return this;
}

Image * Image::pixelizeOffset( float2 v )
{
    this->shading.pixelizeOffset.Set( v );
    return this;
}

Image * Image::resizeStage( FlatShader::ResizeStage v )
{
    this->shading.resizeStage = v;
    return this;
}

Image * Image::resizeAnchor( float2 v )
{
    this->shading.resizeAnchor.Set( v );
    return this;
}

Image * Image::filteringStage( FlatShader::FilteringStage v )
{
    this->shading.filteringStage = v;
    return this;
}

Image * Image::filteringAlphaThreshold( float v )
{
    this->shading.filteringAlphaThreshold.Set( v );
    return this;
}

Image * Image::filteringAlphaWidth( float v )
{
    this->shading.filteringAlphaWidth.Set( v );
    return this;
}

Image * Image::alpha( float v )
{
    this->shading.alpha.Set( v );
    return this;
}

Image * Image::colorTransform( float4x4 const & v )
{
    this->shading.colorTransform.Set( v );
    return this;
}

}
