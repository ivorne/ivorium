#include "Slot.hpp"
#include "../Defs.hpp"
#include "Camera.hpp"

#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

//--------------------- ProtectedSlot --------------------------------
ProtectedSlot::ProtectedSlot( Instance * inst ) :
    Elem( inst ),
    SlotChild( this ),
    cm( inst, this, "ProtectedSlot" ),
    root( inst )
{
    this->cm.inherits( this->Elem::cm, this->SlotChild::cm );
    this->cm.owns( this->root.cm );
    
    this->root.input_setParent( this );
    this->root.elem_setParent( this );
}

void ProtectedSlot::elem_eachChild( std::function< void( Elem * ) > const & f )
{
    f( &this->root );
}

void ProtectedSlot::elem_childDisconnect( Elem * node )
{
}

void ProtectedSlot::input_eachChild( std::function< void( InputNode * ) > const & f )
{
    this->Elem::input_eachChild( f );
    
    f( &this->root );
}

void ProtectedSlot::input_childDisconnect( InputNode * child )
{
    this->Elem::input_childDisconnect( child );
}

void ProtectedSlot::first_pass_impl( ElementRenderer * er )
{
    if( this->expectedSize.dirty() )
    {
        this->expectedSize.clear_dirty();
        this->root.expectedSize.Set( this->expectedSize.Get() );
    }
    
    this->root.elem()->first_pass( er );
    
    if( this->root.preferredSize.dirty() )
        this->root.preferredSize.clear_dirty(),
        this->preferredSize.Set( this->root.preferredSize.Get() );
}

void ProtectedSlot::second_pass_impl( ElementRenderer * er )
{
    bool changed = false;
    
    if( this->modelTransform.dirty() )
        this->modelTransform.clear_dirty(),
        this->root.elem()->modelTransform.Set( this->modelTransform.Get() ),
        changed = true;
    
    if( this->size.dirty() )
        this->size.clear_dirty(),
        this->root.size.Set( this->size.Get() ),
        changed = true;
    
    if( this->scissor.dirty() )
        this->scissor.clear_dirty(),
        this->root.elem()->scissor.Set( this->scissor.Get() ),
        changed = true;
    
    if( changed )
        this->root.elem()->second_pass( er );
}



//--------------------- SlotChild ----------------------
SlotChild::SlotChild( Elem * elem ) :
    Pickable( elem ),
    cm( elem->instance(), this, "SlotChild", ClientMarker::Status() ),
    expectedSize( &this->cm, float3( std::numeric_limits< float >::infinity(), std::numeric_limits< float >::infinity(), std::numeric_limits< float >::infinity() ) ),
    preferredSize( &this->cm, float3( 0, 0, 0 ) ),
    size( &this->cm, float3( 0, 0, 0 ) )
{
    this->cm.inherits( this->Pickable::cm );
}

Instance * SlotChild::instance() const
{
    return this->elem()->instance();
}

void SlotChild::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "SlotChild" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "expectedSize", this->expectedSize.Get() );
    row.Column( "preferredSize", this->preferredSize.Get() );
    row.Column( "size", this->size.Get() );
}

bool SlotChild::picking_test( int2 input_pos )
{
    // check that it was drawn last frame
    if( !this->elem()->elem_getRoot() )
        return false;
    
    // bounding box picking
    float3 size = this->size.Get();
    float2 local_pos = this->elem()->FromScreenPlaneToLocalPlane( input_pos );
    bool is_in = local_pos.x >= 0.0 && local_pos.y >= 0.0 && local_pos.x <= size.x && local_pos.y <= size.y;
    if( !is_in )
    {
        this->cm.log( SRC_INFO, Defs::Log::Picking,
                                "picking_test:", Context::Endl(),
                                "    input_pos=", input_pos, Context::Endl(),
                                "    inner_size=", size, Context::Endl(),
                                "    local_pos=", local_pos, Context::Endl(),
                                "    is_in=", is_in );
        return false;
    }
    
    // scissor test
    ShaderScissor const & scissor = this->elem()->scissor.Get();
    if( scissor.enabled )
    {
        Camera * camera = this->elem()->elem_getRoot();
        if( camera )
        {
            float2 scissor_pos = camera->FromViewportPlaneToLocalPlane( scissor.model, input_pos );
            bool is_in_scissor = scissor_pos.x >= 0.0 && scissor_pos.y >= 0.0 && scissor_pos.x <= scissor.size.x && scissor_pos.y <= scissor.size.y;
            if( !is_in_scissor )
            {
                this->cm.log( SRC_INFO, Defs::Log::Picking,
                                "picking_test:", Context::Endl(),
                                "    input_pos=", input_pos, Context::Endl(),
                                "    inner_size=", size, Context::Endl(),
                                "    local_pos=", local_pos, Context::Endl(),
                                "    is_in=", is_in, Context::Endl(),
                                "    is_in_scissor=", is_in_scissor );
                return false;
            }
        }
    }
    
    // pixel precise picking
    bool is_pixel_picked = this->picking_test_pixel_perfect( local_pos );
    this->cm.log( SRC_INFO, Defs::Log::Picking,
                        "picking_test:", Context::Endl(),
                        "    input_pos=", input_pos, Context::Endl(),
                        "    inner_size=", size, Context::Endl(),
                        "    local_pos=", local_pos, Context::Endl(),
                        "    is_in=", is_in, Context::Endl(),
                        "    is_pixel_picked=", is_pixel_picked );
    return is_pixel_picked;
}

//--------------------- Slot --------------------------------
Slot::Slot( Instance * inst ) :
    VectorChildrenElem< SlotChild >( inst ),
    SlotChild( this ),
    cm( inst, this, "Slot", ClientMarker::Status() )
{
    this->cm.inherits( this->VectorChildrenElem< SlotChild >::cm, this->SlotChild::cm );
}

void Slot::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Slot" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "children", this->children.size() );
}

void Slot::first_pass_impl( ElementRenderer * er )
{
    this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "First pass." );
    
    bool prefsize_changed = this->children_dirty;
    bool expectedSize_changed = this->expectedSize.clear_dirty();
    
    for( SlotChild * child : this->children )
    {
        // expected size to child
        if( expectedSize_changed )
            child->expectedSize.Set( this->expectedSize.Get() );
        
        // first pass
        child->elem()->first_pass( er );
        
        // preferred size change detect
        prefsize_changed = prefsize_changed || child->preferredSize.dirty();
    }
    
    if( prefsize_changed )
    {
        er->Notify_FirstPass_Refresh( this );
        
        float3 prefsize_new( 0 );
        for( SlotChild * child : this->children )
        {
            prefsize_new = glm::max( prefsize_new, child->preferredSize.Get() );
            child->preferredSize.clear_dirty();
        }
        
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass_Refresh, "Refresh preferred_size from ", this->children.size(), " children to ", prefsize_new , "." );
        this->preferredSize.Set( prefsize_new );
    }
    
    if( this->children_dirty || this->size.dirty() || this->modelTransform.dirty() )
    {
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass_Refresh, "Queue second pass." );
        er->QueueSecondPass( this );
    }
}

void Slot::second_pass_impl( ElementRenderer * er )
{
    this->cm.log( SRC_INFO, Defs::Log::ElementSecondPass, "Second pass." );
    
    if( this->children_dirty || this->size.dirty() || this->modelTransform.dirty() || this->scissor.dirty() )
    {
        this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass_Refresh, "Refresh size and model_transform." );
        
        //
        this->children_dirty = false;
        this->size.clear_dirty();
        this->modelTransform.clear_dirty();
        this->scissor.clear_dirty();
        
        //
        for( auto it = this->children.rbegin(); it != this->children.rend(); ++it )
        {
            auto child = *it;
            child->elem()->modelTransform.Set( this->modelTransform.Get() );
            child->elem()->scissor.Set( this->scissor.Get() );
            child->size.Set( this->size.Get() );
            child->elem()->second_pass( er );
        }
    }
}

}
