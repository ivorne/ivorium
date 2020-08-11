#pragma once

#include "Shader.hpp"
#include "Texture.hpp"
#include "CameraState.hpp"
#include "GlSystem.hpp"
#include "../OpenGL/GlMesh.hpp"
#include "../OpenGL/RenderTarget.hpp"
#include <ivorium_systems/ivorium_systems.hpp>
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class FlatShader_Subprovider
{
public:
ClientMarker cm;

                    FlatShader_Subprovider( Instance * inst, VirtualResourceProvider const * );
    void            each_resource( std::function< void( ResourcePath const & ) > const & ) const;
    bool            has_resource( ResourcePath const & ) const;
    
    //
    Instance *      instance() const;
    
private:
    Instance * inst;
};


struct ShaderScissor
{
    float4x4    model;
    float3      size;
    bool        enabled;
    
    //ShaderScissor() : model( ( memset( this, 0, sizeof( ShaderScissor ) ), 1 ) ), size( 0, 0, 0 ), enabled( false ){}
    ShaderScissor() : model( 1 ), size( 0, 0, 0 ), enabled( false ){}
    
    ShaderScissor( ShaderScissor const & right ) :
        //model( ( memset( this, 0, sizeof( ShaderScissor ) ), right.model ) ),
        model( right.model ),
        size( right.size ),
        enabled( right.enabled )
    {}
    
    ShaderScissor const & operator*( ShaderScissor const & right ) const
    {
        return right.enabled ? right : *this;
    }
    
    ShaderScissor & operator*=( ShaderScissor const & right )
    {
        if( right.enabled )
            *this = right;
        return *this;
    }
    
    bool operator==( ShaderScissor const & right ) const
    {
        if( !this->enabled && !right.enabled )
            return true;
        
        return std::tuple( this->enabled, this->model, this->size ) == std::tuple( right.enabled, right.model, right.size );
    }
};


class FlatShader : private GlListener
{
public:
    enum class FittingStage
    {
        None
    };
    
    enum class PixelizeStage
    {
        None,
        Squares,
        Circles
    };

    enum class ResizeStage
    {
        Scale,
        Fixed,
        Repeat,
        Frame
    };
    
    enum class FilteringStage
    {
        Plain,
        Msdf,
        AlphaThreshold,
        AlphaThresholdWidth,
        AlphaThresholdWidthSmooth       ///< Similar to AlphaThresholdWidth, but only center of active alpha range will be fully visible, the farther from center, the lower alpha the fragment will have.
    };
    
    struct Params
    {
        Params( iv::ClientMarker * owner ) :
            fittingStage( FittingStage::None ),
            pixelizeStage( PixelizeStage::None ), pixelizeSize( owner, float2( 1, 1 ) ), pixelizeOffset( owner, float2( 0, 0 ) ),
            resizeStage( ResizeStage::Scale ), resizeAnchor( owner, float2( 0, 0 ) ), 
            filteringStage( FilteringStage::Plain ), filteringAlphaThreshold( owner, 0 ), filteringAlphaWidth( owner, 0 ),
            alpha( owner, 1.0f ),
            colorTransform( owner, float4x4( 1.0f ) )
        {}
        
        // fitting stage
        FittingStage            fittingStage;
        
        // pixelize stage
        PixelizeStage           pixelizeStage;
        DirtyAttr< float2 >     pixelizeSize;               ///< In coordinates after texture resizing (texture_resize parameter in Render method).
        DirtyAttr< float2 >     pixelizeOffset;             ///< In coordinates after texture resizing (texture_resize parameter in Render method).
        
        // resize stage
        ResizeStage             resizeStage;
        DirtyAttr< float2 >     resizeAnchor;
        
        // filtering stage
        FilteringStage          filteringStage;
        DirtyAttr< float >      filteringAlphaThreshold;    ///< If used by filtering_stage, everything with alpha below 0.5 will be always hidden (to not affect smoothing). Pixels between 0.5 and 1.0 will be visible depending on alpha_threshold value, pixels with higher alpha will be visible longer (when transitioning from alpha_threshold 0 to alpha_threshold 1).
        DirtyAttr< float >      filteringAlphaWidth;        ///< Tells use the range of alpha values of texture pixels that will be visible with AlphaThreshold* filtering.
        
        // alpha stage
        DirtyAttr< float >      alpha;
        
        // color stage
        DirtyAttr< float4x4 >   colorTransform;
    };
    
public:
ClientMarker cm;
using GlInfo::instance;

                            FlatShader( Instance * inst, VirtualResourceProvider const *, FlatShader_Subprovider const *, ResourcePath const & path );
    
    /**
        Contains 0 if shader is not currently loaded into gpu (gpu is disabled).
    */
    GLuint                  program_id() const;
    
    /**
        \param log_target
        \param target
        \param camera
        \param mesh
        
        \param mesh_resize
            Resizes input mesh.
            Moves all vertices by multiplying their position by this parameter.
            This scales the texture if the texture_resize stays the same.
            If we scale texture_resize simultaneously with vertex_pos_mul, then the texture render is just resized (not relevant for ResizeStage::Scale, because that one scales either way).
        
        \param mesh_texcoord_density 
            How many texcoords there are on average per distance of 1 in position units (mesh local units).
            Used in pixelize stage and maybe in Msdf for smoothing, not needed in other cases.
            NOTE - This should probably be float3 and should be texel density in three dimensions of the mesh. It wouldn't change nothing in 2D meshes, but will make more sense and might be somewhat useful on 3D meshes. But there is no need until we can test it on something that is in fact useful.
        
        \param texture
        
        \param texture_density_rel
            texture density / screen density
        
        \param texture_msdf_pixelRange
            Used when FilteringStage is msdf.
        
        \param preblend 
            Alpha is usualy 0 to skip preblending or 255 to apply preblending.
            Values in between will apply partial preblending, which can possibly be used as a visual effect (very subtle glow on smoothed renders).
            All translucent texels are blended on top given preblend color.
        
        \param translucent
            Translucent pixels that were not solidified with preblend will be solidified (alpha set to 1 or discard) according to Params::alpha parameter.
        
        \param model
        \param scissor
        \param params
    */
    void                    Render(
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
                            ) const;
    
private:
    virtual void            GlEnable() override;
    virtual void            GlDisable() override;
    virtual void            GlDrop() override;
    
private:
    Shader _shader;
    
    // mesh
    GLint mesh_resize;
    GLint mesh_texcoord_density;
    
    // texture
    GLint tex0;
    GLint tex0_size;
    GLint tex0_density_rel;
    GLint tex0_color_space;
    
    // translucent
    GLint translucent;
    GLint preblend;
    
    // space transform
    GLint projection_view_model;
    GLint local_to_pixel_space;
    GLint pixel_to_local_space;
    GLint depth_override_enabled;
    GLint depth_override;
    
    // scissor
    GLint scissor_enabled;
    GLint local_to_scissor_space;
    GLint scissor_size;
    
    // shader params - fitting stage
    GLint fitting_stage;
    
    // shader params - pixelize stage
    GLint pixelize_stage;
    GLint pixelize_size;
    GLint pixelize_offset;
    
    // shader params - resize stage
    GLint resize_stage;
    GLint resize_anchor;
    
    // shader params - filtering stage
    GLint filtering_stage;
    GLint filtering_msdf_pixel_range;
    GLint filtering_alpha_threshold;
    GLint filtering_alpha_width;
    
    // shader params - color stage
    GLint color_transform;
    
    // shader params - alpha stage
    GLint alpha;
    
    //
    GLint framebuffer_color_space;
};

/**
*/
class FlatShader_Resource : public SingularResource< FlatShader >
{
public:
using SingularResource< FlatShader >::instance;
ClientMarker cm;

    FlatShader_Resource( Instance * inst );
};


//=========================================================================================================
//--------------------------- StringIO --------------------------------------------------------------------
template<>
struct StringIO< FlatShader::FittingStage > : public StringIO_Table< FlatShader::FittingStage >
{
    static const ValuesType Values;
};

template<>
struct StringIO< FlatShader::PixelizeStage > : public StringIO_Table< FlatShader::PixelizeStage >
{
    static const ValuesType Values;
};

template<>
struct StringIO< FlatShader::ResizeStage > : public StringIO_Table< FlatShader::ResizeStage >
{
    static const ValuesType Values;
};

template<>
struct StringIO< FlatShader::FilteringStage > : public StringIO_Table< FlatShader::FilteringStage >
{
    static const ValuesType Values;
};

}
