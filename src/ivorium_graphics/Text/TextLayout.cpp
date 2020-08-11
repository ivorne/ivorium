#include "Text.hpp"

namespace iv
{

//-------------------------- TextSegment -----------------------------------------------------
TextSegment::TextSegment( Elem * elem ) :
    cm( elem->instance(), this, "TextSegment" ),
    geometry_dirty( false ),
    location( &this->cm ),
    _elem( elem )
{
}

Instance * TextSegment::instance() const
{
    return this->_elem->instance();
}

Elem * TextSegment::elem()
{
    return this->_elem;
}

Elem const * TextSegment::elem() const
{
    return this->_elem;
}

//-------------------------- TextLayout -----------------------------------------------------
TextLayout::TextLayout( Instance * inst ) :
    VectorChildrenElem< TextSegment >( inst ),
    SlotChild( this ),
    cm( inst, this, "TextLayout" ),
    attr_lineSpacing( &this->cm, 0.0f ),
    heap(),
    geometry_dirty( false )
{
    this->cm.inherits( this->VectorChildrenElem< TextSegment >::cm, this->SlotChild::cm );
}

TextLayout * TextLayout::line_spacing( float val )
{
    this->attr_lineSpacing.Set( val );
    return this;
}

void TextLayout::first_pass_impl( ElementRenderer * er )
{
    for( TextSegment * child : this->children )
    {
        child->elem()->first_pass( er );
        this->geometry_dirty = this->geometry_dirty || child->geometry_dirty;
        child->geometry_dirty = false;
    }
    
    if( this->geometry_dirty || this->expectedSize.dirty() || this->attr_lineSpacing.dirty() )
    {
        //
        er->Notify_FirstPass_Refresh( this );
        this->expectedSize.clear_dirty();
        this->attr_lineSpacing.clear_dirty();
        
        //
        FontMesh::Location location;
        location.size = float2( this->expectedSize.Get().x, std::numeric_limits< float >::infinity() );
        location.line_spacing = this->attr_lineSpacing.Get();
        location.skip_characters = 0;
        
        float width = 0.0f;
        
        for( TextSegment * child : this->children )
        {
            FontMesh::Geometry geometry = child->geometry_Compute( location );
            location.line_state = geometry.line_state;
            
            width = std::max( width, geometry.max_width );
            
            if( !geometry.fits )
                break;
        }
        
        float height;
        if( location.line_state.baseline_fixed )
            height = location.line_state.basepoint.y - location.line_state.descender;
        else
            height = location.line_state.basepoint.y;
        
        this->preferredSize.Set( float3( width + 0.001, height + 0.001, 0.0f ) );
    }
    
    if( this->geometry_dirty || this->children_dirty || this->size.dirty() || this->modelTransform.dirty() || this->scissor.dirty() )
        er->QueueSecondPass( this );
}

void TextLayout::RepositionChildren()
{
    //
    FontMesh::Location location;
    location.size = float2( this->size.Get().x, this->size.Get().y );
    location.line_spacing = this->attr_lineSpacing.Get();
    location.skip_characters = 0;
    
    bool fits = true;
    
    //
    for( TextSegment * child : this->children )
    {
        if( !fits )
        {
            child->location.Set( FontMesh::Location() );
        }
        else
        {
            // set location
            child->location.Set( location );
            
            // move geometry
            FontMesh::Geometry geometry = child->geometry_Compute( location );
            location.line_state = geometry.line_state;
            fits = fits || geometry.fits;
        }
    }
}

void TextLayout::second_pass_impl( ElementRenderer * er )
{
    //
    bool refresh = false;
    if( this->geometry_dirty || this->children_dirty || this->size.dirty() )
    {
        //
        refresh = true;
        
        //
        er->Notify_SecondPass_Refresh( this );
        
        //
        this->geometry_dirty = false;
        this->children_dirty = false;
        this->size.clear_dirty();
        
        //
        this->RepositionChildren();
    }
    
    if( this->modelTransform.dirty() )
    {
        //
        refresh = true;
        
        //
        this->modelTransform.clear_dirty();
        for( auto child : this->children )
            child->elem()->modelTransform.Set( this->modelTransform.Get() );
    }
    
    if( this->scissor.dirty() )
    {
        this->scissor.clear_dirty();
        for( auto child : this->children )
            child->elem()->scissor.Set( this->scissor.Get() );
    }
    
    if( refresh )
    {
        for( auto it = this->children.rbegin(); it != this->children.rend(); ++it )
        {
            auto child = *it;
            child->elem()->second_pass( er );
        }
    }
}

}
