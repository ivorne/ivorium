#include "Divider.hpp"
#include "../Defs.hpp"

namespace iv
{

//----------------------------------- DividerSlot ----------------------------------
DividerSlot::DividerSlot( Instance * inst ) :
    Slot( inst ),
    cm( inst, this, "DividerSlot", ClientMarker::Status() ),
    attr_lowerOrder( &this->cm, 0 ),
    attr_higherOrder( &this->cm, unsigned( -1 ) )
{
    this->cm.inherits( this->Slot::cm );
}

void DividerSlot::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "DividerSlot" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "lowerOrder", this->attr_lowerOrder.Get() );
    row.Column( "higherOrder", this->attr_higherOrder.Get() );
}

DividerSlot * DividerSlot::lowerOrder( unsigned val )
{
    this->attr_lowerOrder.Set( val );
    return this;
}

DividerSlot * DividerSlot::higherOrder( unsigned val )
{
    this->attr_higherOrder.Set( val );
    return this;
}

//----------------------------------- Divider ----------------------------------
Divider::Divider( Instance * inst ) :
    VectorChildrenElem< DividerSlot >( inst ),
    SlotChild( this ),
    cm( inst, this, "Divider", ClientMarker::Status() ),
    attr_axis( &this->cm, Axis::X ),
    attr_axisOrder( &this->cm, AxisOrder::Incremental ),
    offsets_dirty( true )
{
    this->cm.inherits( this->VectorChildrenElem< DividerSlot >::cm, this->SlotChild::cm );
}

Divider * Divider::axis( Axis val )
{
    this->attr_axis.Set( val );
    return this;
}

Divider * Divider::axisOrder( AxisOrder val )
{
    this->attr_axisOrder.Set( val );
    return this;
}

void Divider::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Divider" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "axis", this->attr_axis.Get() );
    row.Column( "axisOrder", this->attr_axisOrder.Get() );
    row.Column( "children", this->children.size() );
}

void Divider::first_pass_impl( ElementRenderer * er )
{
    this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "First pass." );
    
    //--------- call first pass on children ---------------------------------------
    const bool expectedSize_changed = this->expectedSize.clear_dirty();
    
    bool prefsize_changed = this->children_dirty;
    bool phases_changed = this->children_dirty;
    
    this->children_dirty = false;
    
    for( DividerSlot * child : this->children )
    {
        // expected size to child
        if( expectedSize_changed )
            child->expectedSize.Set( this->expectedSize.Get() );
        
        // first pass
        child->first_pass( er );
        
        // preferred size change detect
        prefsize_changed = prefsize_changed + child->preferredSize.clear_dirty();
        phases_changed = phases_changed + child->attr_lowerOrder.clear_dirty() + child->attr_higherOrder.clear_dirty();
    }
    
    //--------------- refresh preferred size ------------------------------
    if( this->attr_axis.clear_dirty() )
        prefsize_changed = true;
        
    if( prefsize_changed )
    {
        er->Notify_FirstPass_Refresh( this );
        
        float3 prefsize_new( 0 );
        for( SlotChild * child : this->children )
        {
            float3 child_prefsize = child->preferredSize.Get();
            foreach_axis(
                [&]( Axis axis )
                {
                    if( axis == this->attr_axis.Get() )
                        float3_Axis( prefsize_new, axis ) += float3_Axis( child_prefsize, axis );
                    else
                        float3_Axis( prefsize_new, axis ) = std::max( float3_Axis( prefsize_new, axis ), float3_Axis( child_prefsize, axis ) );
                }
            );
        }
        
        //
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass_Refresh, "Refreshed preferred_size: ", this->children.size(), " children with total prefsize ", prefsize_new , "." );
        this->preferredSize.Set( prefsize_new );
    }
    
    //-------------------- refresh phases cache -----------------------------
    if( phases_changed )
    {
        er->Notify_FirstPass_Refresh( this );
        
        this->phases.clear();
        
        for( size_t i = 0; i < this->children.size(); i++ )
        {
            DividerSlot * child = this->children[ i ];
            
            bool first = true;
            
            if( child->attr_lowerOrder.Get() < child->attr_higherOrder.Get() )
            {
                // lower part
                Phase phase;
                phase.segment = i;
                phase.higher = false;
                phase.first = first;
                phase.weight = 0;
                phase.size = 0;
                this->phases.insert( std::pair( child->attr_lowerOrder.Get(), phase ) );
                
                first = false;
            }
            
            // higher part
            Phase phase;
            phase.segment = i;
            phase.higher = true;
            phase.first = first;
            phase.weight = 0;
            phase.size = 0;
            this->phases.insert( std::pair( child->attr_higherOrder.Get(), phase ) );
        }
        
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass_Refresh, "Regenerated phase cache: ", this->children.size(), " children -> ", this->phases.size() , " phases." );
    }
    
    //---------------- recompute sizes and weights -----------------------
    if( phases_changed || prefsize_changed )
    {
        //
        this->offsets_dirty = true;
        
        // cycle over each equal range of phases with same order
        Axis axis = this->attr_axis.Get();
        bool last = false;
        auto it = this->phases.begin();
        size_t groups = 0;
        while( it != this->phases.end() && !last )
        {
            groups += 1;
            
            // get equal range
            auto it_first = it;
            size_t range_size = 0;
            float full_size = 0.0f;
            while( it != this->phases.end() && it->first == it_first->first )
            {
                // read prefsize and process it
                size_t segment = it->second.segment;
                DividerSlot * child = this->children[ segment ];
                float own_size;
                if( !it->second.higher )
                    own_size = float3_Axis( child->preferredSize.Get(), axis );
                else
                    own_size = std::numeric_limits< float >::infinity();
                
                it->second.size = own_size;
                full_size += own_size;
                
                // next
                ++it;
                range_size++;
            }
            
            // fill in weights
            last = full_size == std::numeric_limits< float >::infinity();
            auto it_again = it_first;
            while( it_again != it )
            {
                // compute weight
                if( !last && full_size > 0.0f )
                {
                    it_again->second.weight = it_again->second.size / full_size;
                    full_size -= it_again->second.size;
                }
                else
                {
                    it_again->second.weight = 1.0f / float( range_size );
                }
                
                // next
                ++it_again;
            }
        }
        
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass_Refresh, "Refreshed weights and sizes: ", groups, " groups." );
    }
    
    //-------------- call second pass -------------------------------
    if( this->size.dirty() || this->modelTransform.dirty() || this->scissor.dirty() || this->offsets_dirty || this->attr_axisOrder.dirty() )
    {
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass_Refresh, "Queue second pass." );
        er->QueueSecondPass( this );
    }
}

void Divider::second_pass_impl( ElementRenderer * er )
{
    this->cm.log( SRC_INFO, Defs::Log::ElementSecondPass, "Second pass." );
    
    bool refresh = false;
    
    // inner geometry
    if( this->offsets_dirty || this->size.dirty() )
    {
        this->size.clear_dirty();
        this->offsets_dirty = false;
        
        // notify about refresh
        refresh = true;
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass_Refresh, "Refresh inner geometry." );
        er->Notify_SecondPass_Refresh( this );
        
        // setup sizes
        float3 full_size = this->size.Get();
        Axis axis = this->attr_axis.Get();
        float remaining = float3_Axis( full_size, axis );
        for( auto const & [ order, phase ] : this->phases )
        {
            DividerSlot * child = this->children[ phase.segment ];
            
            // compute size for the item
            float available = phase.weight * remaining;
            float granted = std::min( available, phase.size );
            
            // modify variables
            float3 size = full_size;
            if( phase.first )
                float3_Axis( size, axis ) = 0;
            else
                float3_Axis( size, axis ) = float3_Axis( child->size.Get(), axis );
            
            float3_Axis( size, axis ) += granted;
            
            // modify things
            child->size.Set( size );
            remaining -= granted;
        }
    }
    
    // outer geometry
    if( refresh || this->modelTransform.dirty() || this->attr_axisOrder.dirty() )
    {
        // notify about refresh
        refresh = true;
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass_Refresh, "Refresh outer geometry." );
        
        // clear flags
        this->modelTransform.clear_dirty();
        this->attr_axisOrder.clear_dirty();
        
        // copy to children
        if( this->attr_axisOrder.Get() == AxisOrder::Incremental )
        {
            float offset = 0;
            for( DividerSlot * child : this->children )
            {
                float3 translate( 0 );
                float3_Axis( translate, this->attr_axis.Get() ) = offset;
                offset += float3_Axis( child->size.Get(), this->attr_axis.Get() );
                child->modelTransform.Set( this->modelTransform.Get() * glm::translate( float4x4( 1 ), translate ) );
            }
        }
        else
        {
            float offset = 0;
            for( auto it = this->children.rbegin(); it != this->children.rend(); ++it )
            {
                DividerSlot * child = *it;
                float3 translate( 0 );
                float3_Axis( translate, this->attr_axis.Get() ) = offset;
                offset += float3_Axis( child->size.Get(), this->attr_axis.Get() );
                child->modelTransform.Set( this->modelTransform.Get() * glm::translate( float4x4( 1 ), translate ) );
            }
        }
    }
    
    if( this->scissor.dirty() )
    {
        refresh = true;
        this->scissor.clear_dirty();
        
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass_Refresh, "Refresh scissor." );
        
        for( DividerSlot * child : this->children )
            child->scissor.Set( this->scissor.Get() );
    }
    
    // call second pass on children
    if( refresh )
        for( auto it = this->children.rbegin(); it != this->children.rend(); ++it )
        {
            auto child = *it;
            child->second_pass( er );
        }
}

}
