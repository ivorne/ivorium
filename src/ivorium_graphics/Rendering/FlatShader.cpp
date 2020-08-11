#include "FlatShader.hpp"
#include "ColorTransform.hpp"
#include "../Defs.hpp"
#include <ivorium_config.hpp>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace iv
{

static constexpr char const * FlatShader_Filepath = "/ivorium_graphics/shaders/FlatShader";

//-------------- FlatShader_Resource ---------------------------------------------------------------------
FlatShader_Resource::FlatShader_Resource( Instance * inst ) :
    SingularResource< FlatShader >( inst, FlatShader_Filepath ),
    cm( inst, this, "FlatShader_Resource" )
{
    this->cm.inherits( this->SingularResource< FlatShader >::cm );
}

//-------------- StringIO ---------------------------------------------------------------------
const StringIO< FlatShader::FittingStage >::ValuesType StringIO< FlatShader::FittingStage >::Values = 
{
    { FlatShader::FittingStage::None, "None" }
};

const StringIO< FlatShader::PixelizeStage >::ValuesType StringIO< FlatShader::PixelizeStage >::Values = 
{
    { FlatShader::PixelizeStage::None, "None" },
    { FlatShader::PixelizeStage::Squares, "Squares" },
    { FlatShader::PixelizeStage::Circles, "Circles" }
};

const StringIO< FlatShader::ResizeStage >::ValuesType StringIO< FlatShader::ResizeStage >::Values = 
{
    { FlatShader::ResizeStage::Scale, "Scale" },
    { FlatShader::ResizeStage::Fixed, "Fixed" },
    { FlatShader::ResizeStage::Repeat, "Repeat" },
    { FlatShader::ResizeStage::Frame, "Frame" }
};

const StringIO< FlatShader::FilteringStage >::ValuesType StringIO< FlatShader::FilteringStage >::Values = 
{
    { FlatShader::FilteringStage::Plain, "Plain" },
    { FlatShader::FilteringStage::Msdf, "Msdf" },
    { FlatShader::FilteringStage::AlphaThreshold, "AlphaThreshold" },
    { FlatShader::FilteringStage::AlphaThresholdWidth, "AlphaThresholdWidth" },
    { FlatShader::FilteringStage::AlphaThresholdWidthSmooth, "AlphaThresholdWidthSmooth" },
};

//-------------- FlatShader_Subprovider ---------------------------------------------------------------------
FlatShader_Subprovider::FlatShader_Subprovider( Instance * inst, VirtualResourceProvider const * ) :
    cm( inst, this, "FlatShader_Subprovider" ),
    inst( inst )
{
}

Instance * FlatShader_Subprovider::instance() const
{
    return this->inst;
}

void FlatShader_Subprovider::each_resource( std::function< void( ResourcePath const & ) > const & f ) const
{
    f( ResourcePath( FlatShader_Filepath ) );
}

bool FlatShader_Subprovider::has_resource( ResourcePath const & path ) const
{
    return path == FlatShader_Filepath;
}

//-------------- FlatShader ---------------------------------------------------------------------
FlatShader::FlatShader( Instance * inst, VirtualResourceProvider const *, FlatShader_Subprovider const *, ResourcePath const & path ) :
    GlListener( inst ),
    cm( inst, this, "FlatShader" ),
    _shader( inst, path )
{
    this->cm.owns( this->_shader.cm );
    
    // load shader params
    if( this->GlIsEnabled() )
        this->GlEnable();
}

GLuint FlatShader::program_id() const
{
    return this->_shader.gl_program()->program_id();
}

void FlatShader::GlEnable()
{
    // load shader
    this->_shader.LoadProgram();
    this->_shader.BindAttribute( GlMesh::AttributeLoc_Position, "position" );
    this->_shader.BindAttribute( GlMesh::AttributeLoc_Texcoord, "texcoord" );
    this->_shader.PositionAttributeName( "gl_Position" );
    this->_shader.LinkProgram();
    
    // read uniform positions
    this->mesh_resize = this->_shader.GetUniformLocation( "mesh_resize" );
    this->mesh_texcoord_density = this->_shader.GetUniformLocation( "mesh_texcoord_density" );
    this->tex0 = this->_shader.GetUniformLocation( "tex0" );
    this->tex0_size = this->_shader.GetUniformLocation( "tex0_size" );
    this->tex0_density_rel = this->_shader.GetUniformLocation( "tex0_density_rel" );
    this->tex0_color_space = this->_shader.GetUniformLocation( "tex0_color_space" );
    this->translucent = this->_shader.GetUniformLocation( "translucent" );
    this->preblend = this->_shader.GetUniformLocation( "preblend" );
    this->projection_view_model = this->_shader.GetUniformLocation( "projection_view_model" );
    this->local_to_pixel_space = this->_shader.GetUniformLocation( "local_to_pixel_space" );
    this->pixel_to_local_space = this->_shader.GetUniformLocation( "pixel_to_local_space" );
    this->depth_override = this->_shader.GetUniformLocation( "depth_override" );
    this->depth_override_enabled = this->_shader.GetUniformLocation( "depth_override_enabled" );
    this->scissor_enabled = this->_shader.GetUniformLocation( "scissor_enabled" );
    this->local_to_scissor_space = this->_shader.GetUniformLocation( "local_to_scissor_space" );
    this->scissor_size = this->_shader.GetUniformLocation( "scissor_size" );
    this->fitting_stage = this->_shader.GetUniformLocation( "fitting_stage" );
    this->pixelize_stage = this->_shader.GetUniformLocation( "pixelize_stage" );
    this->pixelize_size = this->_shader.GetUniformLocation( "pixelize_size" );
    this->pixelize_offset = this->_shader.GetUniformLocation( "pixelize_offset" );
    this->resize_stage = this->_shader.GetUniformLocation( "resize_stage" );
    this->resize_anchor = this->_shader.GetUniformLocation( "resize_anchor" );
    this->filtering_stage = this->_shader.GetUniformLocation( "filtering_stage" );
    this->filtering_msdf_pixel_range = this->_shader.GetUniformLocation( "filtering_msdf_pixel_range" );
    this->filtering_alpha_threshold = this->_shader.GetUniformLocation( "filtering_alpha_threshold" );
    this->filtering_alpha_width = this->_shader.GetUniformLocation( "filtering_alpha_width" );
    this->color_transform = this->_shader.GetUniformLocation( "color_transform" );
    this->alpha = this->_shader.GetUniformLocation( "alpha" );
    this->framebuffer_color_space = this->_shader.GetUniformLocation( "framebuffer_color_space" );
}

void FlatShader::GlDisable()
{
    this->_shader.UnloadProgram();
}

void FlatShader::GlDrop()
{
    this->_shader.DropProgram();
}

void FlatShader::Render(
                                ClientMarker const &    log_target,
                                
                                // Renderable
                                CameraState const &     camera,
                                std::optional< float >  depth_override,
                                
                                // Elem
                                float4x4 const &        model,
                                ShaderScissor const &   scissor,
                                
                                // Translucent
                                float4                  preblend,
                                bool                    translucent,
                                
                                // mesh
                                GlMesh const *          mesh,
                                float3                  mesh_resize,
                                float2                  mesh_texcoord_density,
                                
                                // texture
                                GlTexture const *       texture,
                                float                   texture_density_rel,
                                float                   texture_msdf_pixelRange,
                                
                                // parametrization
                                Params const &          params
                        ) const
{
    RenderTarget * target = GlInfo( this->instance() ).render_target();
    
    // texture not loaded yet?
    if( texture->texture_id() == 0 )
    {
        log_target.log( SRC_INFO, Defs::Log::Render, "Texture not loaded, skipping." );
        return;
    }
    
    //==================== bind states ======================================
    target->bind_texture( 0, texture->texture_id() );
    target->bind_shader( this->_shader.gl_program()->program_id() );
    
    //==================== set uniforms =====================================
    // mesh
    glUniform3f( this->mesh_resize, mesh_resize.x, mesh_resize.y, mesh_resize.z );
    glUniform2f( this->mesh_texcoord_density, mesh_texcoord_density.x, mesh_texcoord_density.y );
    
    // texture
    glUniform1i( this->tex0, 0 );
    glUniform2f( this->tex0_size, texture->size().x, texture->size().y );
    glUniform1f( this->tex0_density_rel, texture_density_rel );
    glUniform1i( this->tex0_color_space, (int)texture->gpu_color_space() );
    
    // translucency
    float4 preblend_linear = ColorTransform::sRGB_to_linearRGB( preblend );
    glUniform1i( this->translucent, translucent );
    glUniform4f( this->preblend, preblend_linear.r, preblend_linear.g, preblend_linear.b, preblend.a );
    
    // space transform
    float4x4 projection_view_model = camera.projection * camera.view * model;
    glUniformMatrix4fv( this->projection_view_model, 1, GL_FALSE, glm::value_ptr( projection_view_model ) );
    
    float4x4 local_to_pixel_space = glm::inverse( camera.pixelizing_projection ) * projection_view_model;
    glUniformMatrix4fv( this->local_to_pixel_space, 1, GL_FALSE, glm::value_ptr( local_to_pixel_space ) );
    float4x4 pixel_to_local_space = glm::inverse( local_to_pixel_space );
    glUniformMatrix4fv( this->pixel_to_local_space, 1, GL_FALSE, glm::value_ptr( pixel_to_local_space ) );
    
    glUniform1i( this->depth_override_enabled, depth_override.has_value() );
    glUniform1f( this->depth_override, depth_override.value() );
    
    // scissor
    glUniform1i( this->scissor_enabled, scissor.enabled );
    float4x4 local_to_scissor_space = glm::inverse( scissor.model ) * model;
    glUniformMatrix4fv( this->local_to_scissor_space, 1, GL_FALSE, glm::value_ptr( local_to_scissor_space ) );
    glUniform3f( this->scissor_size, scissor.size.x, scissor.size.y, scissor.size.z );
    
    // fitting
    /*
        Pixel fitting -
        screen_space_position = inv( orthogonal_projection ) * projection * view * model * vertex_position
            - pozice vertexu na displeji, jedna jednotka je jeden pixel
            - jak tuto informaci využít k fittování?
        -> W normalizace pozice
        -> zaokrouhlení pozice
        transformace zpátky:
        result_vertex_position = orthogonal_projection * vec4( adjusted_screen_space_position, 1 )
    */
    glUniform1i( this->fitting_stage, (int)params.fittingStage );
    
    // shader params
    glUniform1i( this->pixelize_stage, (int)params.pixelizeStage );
    glUniform2f( this->pixelize_size, params.pixelizeSize.Get().x, params.pixelizeSize.Get().y );
    glUniform2f( this->pixelize_offset, params.pixelizeOffset.Get().x, params.pixelizeOffset.Get().y );
    
    glUniform1i( this->resize_stage, (int)params.resizeStage );
    glUniform2f( this->resize_anchor, params.resizeAnchor.Get().x, params.resizeAnchor.Get().y );
    
    glUniform1i( this->filtering_stage, (int)params.filteringStage );
    glUniform1f( this->filtering_msdf_pixel_range, texture_msdf_pixelRange );
    glUniform1f( this->filtering_alpha_threshold, params.filteringAlphaThreshold.Get() );
    glUniform1f( this->filtering_alpha_width, params.filteringAlphaWidth.Get() );
    
    glUniformMatrix4fv( this->color_transform, 1, GL_FALSE, glm::value_ptr( params.colorTransform.Get() ) );
    
    glUniform1f( this->alpha, params.alpha.Get() );
    
    #if IV_GLPLATFORM_GLFW
        ColorSpace fb_colorspace = ColorSpace::Linear;
    #elif IV_GLPLATFORM_GLFM
        ColorSpace fb_colorspace = ColorSpace::sRGB;
    #else
        #error "Unimplemented framebuffer color space detection for this GL platform."
    #endif
    glUniform1i( this->framebuffer_color_space, (int)fb_colorspace );
    
    //======================= log =========================================
    log_target.log( SRC_INFO, Defs::Log::Render,
        "\n",
        
        // Renderable
        "    camera.projection ", camera.projection, Context::Endl(),
        "    camera.view ", camera.view, Context::Endl(),
        "    camera.pixelizing_projection ", camera.pixelizing_projection, Context::Endl(),
        "    depth_override ", depth_override, Context::Endl(),
        
        // Elem
        "    model ", model, Context::Endl(),
        "    scissor ", scissor.enabled, " ", scissor.size, " ", scissor.model, Context::Endl(),
        
        // Translucent
        "    translucent ", translucent, Context::Endl(),
        "    preblend ", preblend, Context::Endl(),
        
        // mesh
        "    mesh->indices_cnt ", mesh->indices_cnt(), Context::Endl(),
        "    mesh_resize ", mesh_resize, Context::Endl(),
        "    mesh_texcoord_density ", mesh_texcoord_density, Context::Endl(),
        
        // texture
        "    texture->size ", texture->size(), Context::Endl(),
        "    texture_density_rel ", texture_density_rel, Context::Endl(),
        "    texture_msdf_pixelRange ", texture_msdf_pixelRange, Context::Endl(),
        "    texture_color_space ", texture->gpu_color_space(), Context::Endl(),
        "    framebuffer_color_space ", fb_colorspace, Context::Endl(),
        
        // params
        "    params.fittingStage ", params.fittingStage, Context::Endl(),
        
        "    params.pixelizeStage ", params.pixelizeStage, Context::Endl(),
        "    params.pixelizeSize ", params.pixelizeSize.Get(), Context::Endl(),
        "    params.pixelizeOffset ", params.pixelizeOffset.Get(), Context::Endl(),
        
        "    params.resizeStage ", params.resizeStage, Context::Endl(),
        "    params.resizeAnchor ", params.resizeAnchor.Get(), Context::Endl(),
        
        "    params.filteringStage ", params.filteringStage, Context::Endl(),
        "    params.filteringAlphaThreshold ", params.filteringAlphaThreshold.Get(), Context::Endl(),
        "    params.filteringAlphaWidth ", params.filteringAlphaWidth.Get(), Context::Endl(),
        
        "    params.alpha ", params.alpha.Get(), Context::Endl(),
        
        "    params.colorTransform ", params.colorTransform.Get()
    );
    
    //==================== render call ======================================
    mesh->DrawElements( &log_target );
}

}
