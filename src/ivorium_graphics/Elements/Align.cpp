#include "Align.hpp"

namespace iv
{

Align::Align( Instance * inst ) :
    OneChildElem< SlotChild >( inst ),
    SlotChild( this ),
    cm( inst, this, "Align", ClientMarker::Status() ),
    attr_dontExpand( &this->cm, false ),
    attr_keepAspect( &this->cm, false ),
    attr_resizeScales( &this->cm, false ),
    attr_innerAnchor( &this->cm, float3( 0.5, 0.5, 0.5 ) )
{
    this->cm.inherits( this->OneChildElem< SlotChild >::cm, this->SlotChild::cm );
}

void Align::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Align" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "dontExpand", this->attr_dontExpand.Get() );
    row.Column( "keepAspect", this->attr_keepAspect.Get() );
    row.Column( "resizeScales", this->attr_resizeScales.Get() );
    row.Column( "innerAnchor", this->attr_innerAnchor.Get() );
    row.Column( "insize", this->insize );
    row.Column( "inpos", this->inpos );
    row.Column( "inscale", this->inscale );
}

void Align::first_pass_impl( ElementRenderer * er )
{
    // first pass on child and refresh preferred size
    if( this->child.Get() )
    {
        if( this->child.dirty() || this->expectedSize.dirty() )
        {
            this->expectedSize.clear_dirty();
            this->child.Get()->expectedSize.Set( this->expectedSize.Get() );
        }
        
        this->child.Get()->elem()->first_pass( er );
        
        if( this->child.dirty() || this->child.Get()->preferredSize.dirty() )
        {
            //this->child.Get()->preferred_size.clear_dirty();
            this->preferredSize.Set( this->child.Get()->preferredSize.Get() );
        }
    }
    else
    {
        if( this->child.dirty() )
        {
            this->preferredSize.Set( float3( 0, 0, 0 ) );
        }
    }
    
    if( this->child.Get() )
        if( this->child.dirty() || this->child.Get()->preferredSize.dirty() || this->size.dirty() || this->modelTransform.dirty()
                || this->attr_dontExpand.dirty() || this->attr_keepAspect.dirty() || this->attr_innerAnchor.dirty() || this->scissor.dirty() )
            er->QueueSecondPass( this );
}

void Align::inner_layout( float3 & out_size, float3 & out_position, float3 & out_scale ) const
{
    // prepare size computation
    float3 prefsize = this->preferredSize.Get();
    float3 size = this->size.Get();
    float3 scale( 1, 1, 1 );
    
    // apply dont_expand
    if( this->attr_dontExpand.Get() )
    {
        if( prefsize.x && size.x > prefsize.x )
            size.x = prefsize.x;
        if( prefsize.y && size.y > prefsize.y )
            size.y = prefsize.y;
        if( prefsize.z && size.z > prefsize.z )
            size.z = prefsize.z;
    }
    
    // apply keep_aspect
    if( this->attr_keepAspect.Get() )
    {
        float scale = std::numeric_limits< float >::infinity();
        if( prefsize.x )
            scale = std::min( scale, size.x / prefsize.x );
        if( prefsize.y )
            scale = std::min( scale, size.y / prefsize.y );
        if( prefsize.z )
            scale = std::min( scale, size.z / prefsize.z );
        
        if( prefsize.x )
            size.x = prefsize.x * scale;
        if( prefsize.y )
            size.y = prefsize.y * scale;
        if( prefsize.z )
            size.z = prefsize.z * scale;
    }
    
    // compute inner offset
    float3 position = this->size.Get() - size;
    position *= this->attr_innerAnchor.Get();
    
    // apply resize_scales
    if( this->attr_resizeScales.Get() )
    {
        if( prefsize.x != 0 )
            scale.x = size.x / prefsize.x;
        if( prefsize.y != 0 )
            scale.y = size.y / prefsize.y;
        if( prefsize.z != 0 )
            scale.z = size.z / prefsize.z;
        size = prefsize;
    }
    
    // return values
    out_size = size;
    out_position = position;
    out_scale = scale;
}

void Align::second_pass_impl( ElementRenderer * er )
{
    // delay any refreshes if there is no child
    if( !this->child.Get() )
        return;
    
    // refresh internal geometry
    bool refresh = false;
    if( this->size.dirty() || this->attr_dontExpand.dirty() || this->attr_keepAspect.dirty()
            || this->attr_innerAnchor.dirty() || this->attr_resizeScales.dirty() || this->child.Get()->preferredSize.dirty() )
    {
        er->Notify_SecondPass_Refresh( this );
        refresh = true;
        
        //
        this->inner_layout( this->insize, this->inpos, this->inscale );
        
        //
        this->child.Get()->preferredSize.clear_dirty();
        this->size.clear_dirty();
        this->attr_dontExpand.clear_dirty();
        this->attr_keepAspect.clear_dirty();
        this->attr_innerAnchor.clear_dirty();
        this->attr_resizeScales.clear_dirty();
    }
    
    // reassign values
    if( refresh || this->child.dirty() || this->modelTransform.dirty() || this->scissor.dirty() )
    {
        this->child.clear_dirty();
        this->modelTransform.clear_dirty();
        this->scissor.clear_dirty();
        
        this->child.Get()->elem()->modelTransform.Set( this->modelTransform.Get() * glm::translate( float4x4( 1 ), inpos ) * glm::scale( float4x4( 1 ), this->inscale ) );
        this->child.Get()->elem()->scissor.Set( this->scissor.Get() );
        this->child.Get()->size.Set( insize );
        this->child.Get()->elem()->second_pass( er );
    }
}

Align * Align::enabled( bool val )
{
    this->Elem::enabled( val );
    return this;
}

Align * Align::dontExpand( bool val )
{
    this->attr_dontExpand.Set( val );
    return this;
}

Align * Align::keepAspect( bool val )
{
    this->attr_keepAspect.Set( val );
    return this;
}

Align * Align::innerAnchor( float3 val )
{
    this->attr_innerAnchor.Set( val );
    return this;
}

Align * Align::resizeScales( bool val )
{
    this->attr_resizeScales.Set( val );
    return this;
}

}
