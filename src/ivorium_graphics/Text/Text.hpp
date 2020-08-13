#pragma once

#include "TextLayout.hpp"
#include "../Elements/TranslucentElem.hpp"
#include "../Elements/Elem.hpp"
#include "../Elements/ElementRenderer.hpp"
#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

/**
    \ingroup Elements
    \brief Basic text element, needs to be supplied font path.
*/
class Text : public Elem, public TextSegment, public TranslucentElem, public Renderable
{
public:
ClientMarker cm;
using Elem::instance;
                                    Text( Instance * inst );
    
    void                            status( iv::TableDebugView * view );
    
// initialization parameters
    DirtyAttr< std::string >        attr_text;
    DirtyAttr< ResourcePath >       attr_font;
    DirtyAttr< float >              attr_fontSize;
    DirtyAttr< float4 >             attr_color;
    //DirtyAttr< bool >             attr_fixedAllowed;
    
// utils
    // Elem
    Text *                          enabled( bool );
    
    // TranslucentElem
    Text *                          preblend( float4 val );
    Text *                          translucent( bool val );
    
    // Text
    Text *                          text( std::string const & );
    Text *                          font( ResourcePath const & );
    Text *                          fontSize( float );
    Text *                          color( float4 );
    //Text *                            fixedAllowed( bool );
    
//
    Text *                          font_DVar( DVarIdT< ResourcePath > id );
    
protected:
// Elem
    virtual void                    first_pass_impl( ElementRenderer * ) override;
    virtual void                    second_pass_impl( ElementRenderer * ) override;
    
private:
// TextSegment
    virtual FontMesh::Geometry      geometry_Compute( FontMesh::Location const & location ) override;
    
// Renderable
    virtual void                    render( CameraState const & camera, std::optional< float > depth_override ) override;
    
private:
    FontMesh mesh;
    Heap heap;
};


}
