#include "Transform.hpp"

#include "../Defs.hpp"

namespace iv
{

Transform::Transform( Instance * inst ) :
    OneChildElem< SlotChild >( inst ),
    SlotChild( this ),
    cm( inst, this, "Transform", ClientMarker::Status() ),
    attr_position( &this->cm, float3( 0, 0, 0 ) ),
    attr_rotation( &this->cm, floatQuat() ),
    attr_scale( &this->cm, float3( 1, 1, 1 ) ),
    attr_deformation( &this->cm, float4x4( 1 ) ),
    attr_outerAnchor( &this->cm, float3( 0.5f, 0.5f, 0.5f ) ),
    attr_innerAnchor( &this->cm, float3( 0.5f, 0.5f, 0.5f ) ),
    attr_propagateSize( &this->cm, false ),
    outer_transform_refresh( false ),
    inner_transform_refresh( false )
{
    this->cm.inherits( this->OneChildElem< SlotChild >::cm, this->SlotChild::cm );
}

void Transform::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Transform" );
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "position", this->attr_position.Get() );
    row.Column( "rotation", this->attr_rotation.Get() );
    row.Column( "scale", this->attr_scale.Get() );
    row.Column( "deformation", this->attr_deformation.Get() );
    
    row.Column( "outerAnchor", this->attr_outerAnchor.Get() );
    row.Column( "innerAnchor", this->attr_innerAnchor.Get() );
    
    row.Column( "propagateSize", this->attr_propagateSize.Get() );
}

void Transform::first_pass_impl( ElementRenderer * er )
{
    this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "First pass." );
    
    // first pass on child and refresh preferred size
    if( this->child.Get() )
    {
        if( this->child.dirty() || this->expectedSize.dirty() || this->attr_propagateSize.dirty() )
        {
            this->expectedSize.clear_dirty();
            
            if( this->attr_propagateSize.Get() )
                this->child.Get()->expectedSize.Set( this->expectedSize.Get() );
            else
                this->child.Get()->expectedSize.Set( float3( std::numeric_limits< float >::infinity(), std::numeric_limits< float >::infinity(), std::numeric_limits< float >::infinity() ) );
        }
        
        this->child.Get()->elem()->first_pass( er );
        
        if( this->child.clear_dirty() || this->attr_propagateSize.clear_dirty() || this->child.Get()->preferredSize.clear_dirty() || this->attr_deformation.clear_dirty() || this->attr_innerAnchor.clear_dirty() )
        {
            this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "Refresh preferred_size, set inner_transform_refresh." );
            
            if( this->attr_propagateSize.Get() )
            {
                this->child.Get()->preferredSize.clear_dirty();
                this->preferredSize.Set( this->child.Get()->preferredSize.Get() );
            }
            else
            {
                this->preferredSize.Set( float3( 0, 0, 0 ) );
            }
            
            this->inner_transform_refresh = true;
        }
    }
    else
    {
        if( this->child.dirty() )
        {
            this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "Refresh preferred_size." );
            this->preferredSize.Set( float3( 0, 0, 0 ) );
        }
    }
    
    if( this->child.Get() )
    {
        if( this->attr_position.clear_dirty() || this->attr_rotation.clear_dirty() || this->attr_scale.clear_dirty() || this->attr_outerAnchor.clear_dirty() )
        {
            this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "Set outer_transform_refresh." );
            
            
            //
            this->outer_transform_refresh = true;
        }
        
        if( this->child.dirty() || this->inner_transform_refresh || this->outer_transform_refresh || this->modelTransform.dirty() || this->size.dirty() || this->scissor.dirty() )
        {
            this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "Queue second pass." );
            er->QueueSecondPass( this );
        }
    }
}

void Transform::second_pass_impl( ElementRenderer * er )
{
    this->cm.log( SRC_INFO, Defs::Log::ElementSecondPass, "Second pass." );
    
    // delay any refreshes if there is no child
    if( !this->child.Get() )
        return;
    
    bool refresh = false;
    
    // inner transform
    if( this->inner_transform_refresh || this->size.dirty() )
    {
        this->cm.log( SRC_INFO, Defs::Log::ElementSecondPass, "Refresh inner transform." );
        
        //
        this->inner_transform_refresh = false;
        
        //
        refresh = true;
        
        //
        if( this->attr_propagateSize.Get() )
            this->child.Get()->size.Set( this->size.Get() );
        else
            this->child.Get()->size.Set( this->child.Get()->preferredSize.Get() );
        
        this->inner_transform = glm::translate( float4x4( 1 ), float3() - this->child.Get()->size.Get() * this->attr_innerAnchor.Get() ) * this->attr_deformation.Get();
    }
    
    // outer transform
    if( this->outer_transform_refresh || this->size.clear_dirty() )
    {
        this->outer_transform_refresh = false;
        
        this->cm.log( SRC_INFO, Defs::Log::ElementSecondPass, "Refresh outer transform." );
        
        //
        refresh = true;
        
        //
        this->outer_transform = glm::translate( float4x4( 1 ), this->size.Get() * this->attr_outerAnchor.Get() )
                                * glm::translate( float4x4( 1 ), this->attr_position.Get() )
                                * glm::mat4_cast( this->attr_rotation.Get() )
                                * glm::scale( float4x4( 1 ), this->attr_scale.Get() );
    }
    
    // refresh
    if( refresh || this->child.clear_dirty() || this->modelTransform.clear_dirty() || this->scissor.clear_dirty()  )
    {
        this->cm.log( SRC_INFO, Defs::Log::ElementSecondPass, "Apply transforms." );
        
        er->Notify_SecondPass_Refresh( this );
        
        this->child.Get()->elem()->modelTransform.Set( this->modelTransform.Get() * this->outer_transform * this->inner_transform );
        this->child.Get()->elem()->scissor.Set( this->scissor.Get() );
        this->child.Get()->elem()->second_pass( er );
    }
}

Transform * Transform::position( float3 val )
{
    this->attr_position.Set( val );
    return this;
}

Transform * Transform::rotation( floatQuat val )
{
    this->attr_rotation.Set( val );
    return this;
}

Transform * Transform::scale( float3 val )
{
    this->attr_scale.Set( val );
    return this;
}

Transform * Transform::deformation( float4x4 val )
{
    this->attr_deformation.Set( val );
    return this;
}

Transform * Transform::outerAnchor( float3 val )
{
    this->attr_outerAnchor.Set( val );
    return this;
}

Transform * Transform::innerAnchor( float3 val )
{
    this->attr_innerAnchor.Set( val );
    return this;
}

Transform * Transform::propagateSize( bool val )
{
    this->attr_propagateSize.Set( val );
    return this;
}

}
