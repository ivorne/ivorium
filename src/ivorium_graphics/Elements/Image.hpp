#pragma once

#include "Elem.hpp"
#include "Slot.hpp"
#include "TranslucentElem.hpp"
#include "../Rendering/Texture.hpp"
#include "../Rendering/FlatShader.hpp"
#include "../Rendering/SquareMesh.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
*/
class Image : public Elem, public SlotChild, public TranslucentElem, public Renderable
{
public:
ClientMarker cm;
using Elem::instance;
                                Image( Instance * );
    
    void                        status( iv::TableDebugView * view );
    
    // Renderable
    virtual void                render( CameraState const & camera, std::optional< float > depth_override ) override;
    
// initialization parameters
    DirtyAttr< ResourcePath >   attr_filename;
    FlatShader::Params          shading;
    
// utility methods
    // Elem
    Image *                     enabled( bool val );
    
    // TranslucentElem
    Image *                     preblend( float4 val );
    Image *                     translucent( bool val );
    
    // Image
    Image *                     filename( ResourcePath const & val );
    
    // Image shader params
    Image *                     fittingStage( FlatShader::FittingStage v );
    
    Image *                     pixelizeStage( FlatShader::PixelizeStage v );
    Image *                     pixelizeSize( float2 v );
    Image *                     pixelizeOffset( float2 v );
    
    Image *                     resizeStage( FlatShader::ResizeStage v );
    Image *                     resizeAnchor( float2 v );
    
    Image *                     filteringStage( FlatShader::FilteringStage v );
    Image *                     filteringAlphaThreshold( float v );
    Image *                     filteringAlphaWidth( float v );
    Image *                     alpha( float v );
    
    Image *                     colorTransform( float4x4 const & v );
    
protected:
    // Elem
    virtual void                first_pass_impl( ElementRenderer * ) override;
    virtual void                second_pass_impl( ElementRenderer * ) override;
    
    // SlotChild
    virtual bool                picking_test_pixel_perfect( float2 local_pos ) override;
    
private:
    static float                texcoord_frame_transform_px( float coord_image_px, float tex_size, float image_size );
    
private:
    Texture_Resource texture;
    FlatShader_Resource shader;
    SquareMesh_Resource square;
};

}
