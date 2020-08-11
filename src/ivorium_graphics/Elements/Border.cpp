#include "Border.hpp"

#include "Axis.hpp"

namespace iv
{

Border::Border( Instance * inst ) :
    OneChildElem< SlotChild >( inst ),
    SlotChild( this ),
    cm( inst, this, "Border", ClientMarker::Status() ),
    attr_borderLeft( &this->cm ),
    attr_borderRight( &this->cm ),
    attr_borderTop( &this->cm ),
    attr_borderBottom( &this->cm ),
    attr_borderFront( &this->cm ),
    attr_borderBack( &this->cm ),
    insizeDirty( false )
{
    this->cm.inherits( this->OneChildElem< SlotChild >::cm, this->SlotChild::cm );
}

void Border::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Border" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "borderLeft", this->attr_borderLeft.Get() );
    row.Column( "borderRight", this->attr_borderRight.Get() );
    row.Column( "borderTop", this->attr_borderTop.Get() );
    row.Column( "borderBottom", this->attr_borderBottom.Get() );
    row.Column( "borderFront", this->attr_borderFront.Get() );
    row.Column( "borderBack", this->attr_borderBack.Get() );
}

float3 Border::BorderSums() const
{
    float3 sum;
    sum.x = this->attr_borderLeft.Get().value_or( 0.0f ) + this->attr_borderRight.Get().value_or( 0.0f );
    sum.y = this->attr_borderTop.Get().value_or( 0.0f ) + this->attr_borderBottom.Get().value_or( 0.0f );
    sum.z = this->attr_borderFront.Get().value_or( 0.0f ) + this->attr_borderBack.Get().value_or( 0.0f );
    return sum;
}

void Border::first_pass_impl( ElementRenderer * er )
{
    bool border_changed = this->attr_borderLeft.dirty() || this->attr_borderRight.dirty()
                            || this->attr_borderTop.dirty() || this->attr_borderBottom.dirty()
                            || this->attr_borderFront.dirty() || this->attr_borderBack.dirty();
    
    //
    float3 child_prefsize;
    bool child_prefsize_changed = false;
    
    if( this->child.Get() )
    {
        if( this->child.dirty() || this->expectedSize.dirty() || border_changed )
        {
            this->expectedSize.clear_dirty();
            this->child.Get()->expectedSize.Set( this->expectedSize.Get() - this->BorderSums() );
        }
        
        this->child.Get()->elem()->first_pass( er );
        
        if( this->child.dirty() || this->child.Get()->preferredSize.dirty() )
        {
            this->child.Get()->preferredSize.clear_dirty();
            child_prefsize_changed = true;
        }
    }
    else if( this->child.dirty() )
    {
        child_prefsize_changed = true;
    }
    
    if( child_prefsize_changed )
        this->insizeDirty = true;
    
    if( child_prefsize_changed || border_changed )
    {
        float3 child_prefsize( 0, 0, 0 );
        if( this->child.Get() )
            child_prefsize = this->child.Get()->preferredSize.Get();
        
        this->preferredSize.Set( child_prefsize + this->BorderSums() );
    }
    
    if( this->child.Get() )
        if( this->child.dirty() || border_changed || this->insizeDirty || this->scissor.dirty() )
            er->QueueSecondPass( this );
}

void Border::inner_layout( float3 & out_size, float3 & out_position ) const
{
    // prepare size computation
    float3 prefsize = this->child.Get()->preferredSize.Get();
    float3 size = this->size.Get();
    
    //
    bool fail = false;
    
    // X
    float borders_x_sum = this->attr_borderLeft.Get().value_or(0) + this->attr_borderRight.Get().value_or( 0 );
    if( fail || borders_x_sum > size.x )
    {
        fail = true;
    }
    else
    {
        float inspace = size.x - borders_x_sum;
        if( this->attr_borderLeft.Get().has_value() && this->attr_borderRight.Get().has_value() )
        {
            out_size.x = inspace;
            out_position.x = this->attr_borderLeft.Get().value();
        }
        else
        {
            out_size.x = std::min( inspace, prefsize.x );
            
            if( this->attr_borderLeft.Get().has_value() )
                out_position.x = this->attr_borderLeft.Get().value();
            else if( this->attr_borderRight.Get().has_value() )
                out_position.x = size.x - this->attr_borderRight.Get().value() - out_size.x;
            else
                out_position.x = ( size.x - out_size.x ) / 2.0f;
        }
    }
    
    // Y
    float borders_y_sum = this->attr_borderTop.Get().value_or(0) + this->attr_borderBottom.Get().value_or(0);
    if( fail || borders_y_sum > size.y )
    {
        fail = true;
    }
    else
    {
        float inspace = size.y - borders_y_sum;
        if( this->attr_borderTop.Get().has_value() && this->attr_borderBottom.Get().has_value() )
        {
            out_size.y = inspace;
            out_position.y = this->attr_borderTop.Get().value();
        }
        else
        {
            out_size.y = std::min( inspace, prefsize.y );
            
            if( this->attr_borderTop.Get().has_value() )
                out_position.y = this->attr_borderTop.Get().value();
            else if( this->attr_borderBottom.Get().has_value() )
                out_position.y = size.y - this->attr_borderBottom.Get().value() - out_size.y;
            else
                out_position.y = ( size.y - out_size.y ) / 2.0f;
        }
    }
    
    // Z
    float borders_z_sum = this->attr_borderFront.Get().value_or(0) + this->attr_borderBack.Get().value_or(0);
    if( fail || borders_z_sum > size.z )
    {
        fail = true;
    }
    else
    {
        float inspace = size.z - borders_z_sum;
        if( this->attr_borderFront.Get().has_value() && this->attr_borderBack.Get().has_value() )
        {
            out_size.z = inspace;
            out_position.z = this->attr_borderFront.Get().value();
        }
        else
        {
            out_size.z = std::min( inspace, prefsize.z );
            
            if( this->attr_borderFront.Get().has_value() )
                out_position.z = this->attr_borderFront.Get().value();
            else if( this->attr_borderBack.Get().has_value() )
                out_position.z = size.z - this->attr_borderBack.Get().value() - out_size.z;
            else
                out_position.z = ( size.z - out_size.z ) / 2.0f;
        }
    }
    
    if( fail )
    {
        out_size = float3( 0, 0, 0 );
        out_position = float3( 0, 0, 0 );
    }
}

void Border::second_pass_impl( ElementRenderer * er )
{
    // delay any refreshes if there is no child
    if( !this->child.Get() )
        return;
    
    bool refresh = false;
    
    // refresh internal geometry
    bool border_changed = this->attr_borderLeft.clear_dirty() + this->attr_borderRight.clear_dirty()
                            + this->attr_borderTop.clear_dirty() + this->attr_borderBottom.clear_dirty()
                            + this->attr_borderFront.clear_dirty() + this->attr_borderBack.clear_dirty();
    
    if( this->size.clear_dirty() + border_changed + this->insizeDirty )
    {
        this->insizeDirty = false;
        
        //
        er->Notify_SecondPass_Refresh( this );
        refresh = true;
        
        //
        this->inner_layout( this->insize, this->inpos );
    }
    
    //
    if( refresh || this->child.dirty() || this->modelTransform.dirty() || this->scissor.dirty() )
    {
        this->child.clear_dirty();
        this->modelTransform.clear_dirty();
        this->scissor.clear_dirty();
        
        this->child.Get()->elem()->modelTransform.Set( this->modelTransform.Get() * glm::translate( float4x4( 1 ), inpos ) );
        this->child.Get()->elem()->scissor.Set( this->scissor.Get() );
        this->child.Get()->size.Set( insize );
        this->child.Get()->elem()->second_pass( er );
    }
}

Border * Border::enabled( bool val )
{
    this->Elem::enabled( val );
    return this;
}

Border * Border::borderLeft( std::optional< float > val )
{
    this->attr_borderLeft.Set( val );
    return this;
}

Border * Border::borderRight( std::optional< float > val )
{
    this->attr_borderRight.Set( val );
    return this;
}

Border * Border::borderTop( std::optional< float > val )
{
    this->attr_borderTop.Set( val );
    return this;
}

Border * Border::borderBottom( std::optional< float > val )
{
    this->attr_borderBottom.Set( val );
    return this;
}

Border * Border::borderFront( std::optional< float > val )
{
    this->attr_borderFront.Set( val );
    return this;
}

Border * Border::borderBack( std::optional< float > val )
{
    this->attr_borderBack.Set( val );
    return this;
}

Border * Border::leftTop( std::optional< float > left, std::optional< float > top )
{
    this->attr_borderLeft.Set( left );
    this->attr_borderTop.Set( top );
    return this;
}

Border * Border::leftBottom( std::optional< float > left, std::optional< float > bottom )
{
    this->attr_borderLeft.Set( left );
    this->attr_borderBottom.Set( bottom );
    return this;
}

Border * Border::rightTop( std::optional< float > right, std::optional< float > top )
{
    this->attr_borderRight.Set( right );
    this->attr_borderTop.Set( top );
    return this;
}

Border * Border::rightBottom( std::optional< float > right, std::optional< float > bottom )
{
    this->attr_borderRight.Set( right );
    this->attr_borderBottom.Set( bottom );
    return this;
}

Border * Border::leftRight( std::optional< float > left, std::optional< float > right )
{
    this->attr_borderLeft.Set( left );
    this->attr_borderRight.Set( right );
    return this;
}

Border * Border::topBottom( std::optional< float > top, std::optional< float > bottom )
{
    this->attr_borderTop.Set( top );
    this->attr_borderBottom.Set( bottom );
    return this;
}

Border * Border::frontBack( std::optional< float > front, std::optional< float > back )
{
    this->attr_borderFront.Set( front );
    this->attr_borderBack.Set( back );
    return this;
}

}
