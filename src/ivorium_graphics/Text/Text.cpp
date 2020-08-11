#include "Text.hpp"

#include <ivorium_core/ivorium_core.hpp>

#warning "TODO - Text currently uses only MSDF variant of the text. Details about the problem and possible solutions are in comment below."
/*
    The problem is that texture_id is set in first_pass but variants (and therefore texture_id) would be selected in second_pass.
    One solution would be to have concept of unknown texture_ids in render queues, so that the sorting will be a bit problematic, but it would work.
    Other solution is to have some kind of permanency of pointers in render queues - so that we would not need to add renderables
        to render queues each frame (in first_pass) but only when their properties change (so that it can be done in second_pass which is not called each frame).
    One problem with that would be the fact that we recompute draw order each frame in initialization part of first_pass - that is changed each frame,
        so that needs to be set in first_pass - but maybe render queue could query that value during sorting phase of render."
    Either way - lets test and profile and improve the element graph rendering before implementing this.
    
    -> Probably use the 'unknown' texture_id for now.
     -> Or make renderer ask each instance for texture_id right before sorting, in render pass, after second pass.
*/

namespace iv
{

Text::Text( Instance * inst ) :
    Elem( inst ),
    TextSegment( this ),
    TranslucentElem( this ),
    cm( inst, this, "Text", ClientMarker::Status() ),
    attr_text( &this->cm, "" ),
    attr_font( &this->cm, "" ),
    attr_fontSize( &this->cm, 12 ),
    attr_color( &this->cm, float4( 0.0f, 0.0f, 0.0f, 1.0f ) ),
    mesh( inst ),
    heap( inst, &this->cm )
{
    this->cm.inherits( this->Elem::cm, this->TextSegment::cm, this->TranslucentElem::cm );
    this->cm.owns( this->mesh.cm );
}

void Text::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Text" );
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "text", this->attr_text.Get() );
    row.Column( "font", this->attr_font.Get() );
    row.Column( "font_size", this->attr_fontSize.Get() );
    row.Column( "color", this->attr_color.Get() );
}

Text * Text::font_DVar( DVarIdT< ResourcePath > id )
{
    this->heap.createClient< DVar_Field< ResourcePath > >( id )
        ->Assign_Attribute_RW( &this->attr_font );
    
    return this;
}


Text * Text::enabled( bool val )
{
    this->Elem::enabled( val );
    return this;
}

Text * Text::preblend( float4 val )
{
    this->TranslucentElem::preblend( val );
    return this;
}

Text * Text::translucent( bool val )
{
    this->TranslucentElem::translucent( val );
    return this;
}

Text * Text::text( std::string const & val )
{
    this->attr_text.Set( val );
    return this;
}

Text * Text::font( ResourcePath const & val )
{
    this->attr_font.Set( val );
    return this;
}

Text * Text::fontSize( float val )
{
    this->attr_fontSize.Set( val );
    return this;
}

Text * Text::color( float4 val )
{
    this->attr_color.Set( val );
    return this;
}
/*
Text * Text::fixed_enabled_Set( bool val )
{
    this->fixed_enabled.Set( val );
    return this;
}
*/

void Text::first_pass_impl( ElementRenderer * er )
{
    if( this->attr_font.dirty() )
    {
        this->mesh.font_path( this->attr_font.Get() );
    }
    
    if( this->attr_text.dirty() || this->attr_font.dirty() || this->attr_fontSize.dirty() )
    {
        er->QueueSecondPass( this );
        this->geometry_dirty = true;
    }
    
    // queue render
    {
        GLuint shader_id = this->mesh.program_id();
        GLuint texture_id = this->mesh.texture_id();
        
        if( shader_id && texture_id )
        {
            if( this->attr_translucent.Get() )
                er->AddRenderable_Translucent( this, shader_id, texture_id );
            else
                er->AddRenderable_Solid( this, shader_id, texture_id );
        }
    }
}

FontMesh::Geometry Text::geometry_Compute( FontMesh::Location const & location )
{
    return this->mesh.ComputeGeometry( this->attr_text.Get(), this->attr_fontSize.Get(), location );
}

void Text::second_pass_impl( ElementRenderer * er )
{
    // refresh meshes
    if( this->attr_text.clear_dirty() || this->attr_font.clear_dirty() || this->attr_fontSize.clear_dirty()
            || this->location.clear_dirty() /*|| this->attr_fixedAllowed.clear_dirty()*/ )
    {
        //
        er->Notify_SecondPass_Refresh( this );
        
        //
        // TODO - maybe implement selection of font variant
        this->mesh.GenerateMesh( this->attr_text.Get(), this->attr_fontSize.Get(), this->location.Get() );
    }
    
    if( this->modelTransform.dirty() )
    {
        // clear
        this->modelTransform.clear_dirty();
        
        //
        // TODO - maybe implement selection of font variant - regenerate mesh if selected variant for given model_transform changed
    }
}

void Text::render( CameraState const & camera, std::optional< float > depth_override )
{
    this->mesh.Render( camera, this->modelTransform.Get(), this->attr_color.Get(), this->attr_preblend.Get(), this->attr_translucent.Get(), depth_override, this->scissor.Get() );
}

}
