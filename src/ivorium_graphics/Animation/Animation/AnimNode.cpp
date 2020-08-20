#include "AnimNode.hpp"
#include "AnimSystem.hpp"

namespace iv
{

TableId AnimNode_DebugTable  = TableId::create( "AnimNode" );

AnimNodeI::AnimNodeI( Instance * inst ) :
    cm( inst, this, "AnimNode", ClientMarker::Status() ),
    inst( inst ),
    as( inst->getSystem< AnimSystem >() ),
    _label(),
    _speed( 1.0 ),
    _anim_initialized( false ),
    _parent( nullptr ),
    _children(),
    _activation_id( std::nullopt ),
    _active_cooldown( 0 ),
    _active( false ),
    _distance_start( 0 ),
    _distance_end( 0 ),
    _distance_working( 0 ),
    _requested_step( 0 ),
    _update_id( std::nullopt ),
    in_target_notified( false )
{
    if( auto as = this->instance()->getSystem< AnimSystem >() )
        as->InsertRoot( this );
}

AnimNodeI::~AnimNodeI()
{
    if( this->_parent )
        this->_parent->anim_childDisconnect( this );
    else if( auto as = this->instance()->getSystem< AnimSystem >() )
        as->RemoveRoot( this );
    
    for( AnimConnector * child : this->_children )
        child->anim_parentDisconnect( this );
}

Instance * AnimNodeI::instance() const
{
    return this->inst;
}

void AnimNodeI::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "AnimNodeI" );
    
    auto row = view->Table( DebugTable ).Row( this );
    row.Column( "label", this->label() );
    row.Column( "speed", this->Speed() );
    row.Column( "distance", this->Distance() );
    row.Column( "parent/cnt", bool( this->_parent ) );
    row.Column( "children/cnt", this->_children.size() );
}

AnimNodeI * AnimNodeI::label( std::string const & val )
{
    this->_label = val;
    return this;
}

std::string const & AnimNodeI::label() const
{
    return this->_label;
}

//====================================================================================
//------------------------------- Tree structure -------------------------------------
void AnimNodeI::anim_setParent( AnimConnector * parent )
{
    if( this->_parent )
    {
        this->cm.warning( SRC_INFO, "Can not set parent connector, nodes shoud only have one parent." );
        return;
    }
    
    if( !parent )
    {
        this->cm.warning( SRC_INFO, "Can not set nullptr parent connector, use anim_unsetParent instead." );
        return;
    }
    
    if( auto as = this->instance()->getSystem< AnimSystem >() )
        as->RemoveRoot( this );
    
    this->_parent = parent;
}

void AnimNodeI::anim_unsetParent( AnimConnector * parent )
{
    if( !this->_parent )
    {
        this->cm.warning( SRC_INFO, "Can not unset parent connector, this node currently has no parent." );
        return;
    }
    
    if( parent != this->_parent )
    {
        this->cm.warning( SRC_INFO, "Can not unset parent connector, this node has different parent." );
        return;
    }
    
    if( auto as = this->instance()->getSystem< AnimSystem >() )
        as->RemoveRoot( this );
    
    this->_parent = nullptr;
}

AnimConnector * AnimNodeI::anim_getParent()
{
    return this->_parent;
}

void AnimNodeI::anim_addChild( AnimConnector * child )
{
    this->_children.insert( child );
}

void AnimNodeI::anim_removeChild( AnimConnector * child )
{
    this->_children.erase( child );
}

void AnimNodeI::anim_eachChild( std::function< void( AnimConnector * ) > const & f )
{
    for( AnimConnector * child : this->_children )
        f( child );
}

//============================================================================================================
//-------------------------- Animation update ----------------------------------------------------------------
void AnimNodeI::System_UpdateStart()
{
    //
    this->_distance_start = this->_distance_end;
    this->_distance_working = 0.0f;
    this->_requested_step = 0.0f;
    this->_update_id = this->as->update_id();
    
    //
    this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_NodeUpdate, "UpdateStart | distance ",this->_distance_start, "." );
}

void AnimNodeI::System_UpdateEnd()
{
    //
    this->_distance_end = this->_distance_working;
    this->_update_id = std::nullopt;
    this->UpdateLastTarget();
    
    //
    bool in_target = this->IsInTarget();
    
    //
    this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_NodeUpdate, "UpdateEnd | distance ", this->_distance_start," -> ", this->_distance_end, ", in_target ", in_target, "." );
    
    // inform parent connector about arrival to destination
    if( in_target )
    {
        if( !this->in_target_notified )
            if( AnimConnector * parent = this->anim_getParent() )
                parent->childArrived_Set();
    }
    else
    {
        this->in_target_notified = false;
    }
    
    // maybe deactivate node
    if( in_target )
    {
        if( this->_active_cooldown > 1 )
        {
            this->_active_cooldown -= 1;
        }
        else
        {
            if( this->_active )
                this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_Activations, "Deactivate node." );
            
            this->_active = false;
            this->_active_cooldown = 0;
        }
    }
    else
    {
        if( !this->_active )
            this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_Activations, "Activate node." );
        this->_active = true;
        this->_active_cooldown = ActiveCooldownFrames;
    }
}

std::optional< unsigned > AnimNodeI::System_UpdateId() const
{
    return this->_update_id;
}

void AnimNodeI::Connector_RequestStep( Anim_float step_distance )
{
    this->_requested_step = step_distance * this->Speed();
}

void AnimNodeI::Connector_UpdateDistance( Anim_float new_distance )
{
    // scale distance
    Anim_float new_distance_scaled = new_distance / this->Speed();
    if( this->Speed() == std::numeric_limits< Anim_float >::infinity() )
        new_distance_scaled = 0.0;
    
    // update distance
    this->_distance_working = std::max( this->_distance_working, new_distance_scaled );
}

Anim_float AnimNodeI::Step()
{
    if( !this->_anim_initialized )
    {
        this->_anim_initialized = true;
        return std::numeric_limits< Anim_float >::infinity();
    }
    
    return this->_requested_step;
}

bool AnimNodeI::Active()
{
    if( !this->_active && this->_activation_id.has_value() )
    {
        unsigned update_id = this->as->update_id();
        if( this->_activation_id.value() == update_id )
        {
            this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_Activations, "Activate node." );
            this->_activation_id = std::nullopt;
            this->_active = true;
            this->_active_cooldown = ActiveCooldownFrames;
        }
    }
    
    return this->_active;
}

void AnimNodeI::Activate()
{
    // queue activation for next frame
    if( !this->Active() )
        this->_activation_id = this->as->update_id() + 1;
    else
        this->_active_cooldown = ActiveCooldownFrames;
    
    // activate parent nodes
    if( AnimConnector * parent = this->anim_getParent() )
    {
        parent->anim_eachParent(
            [&]( AnimNodeI * node )
            {
                node->Activate();
            }
        );
    }
}

Anim_float AnimNodeI::Distance()
{
    return this->_distance_end;
}

Anim_float AnimNodeI::Speed() const
{
    return this->_speed;
}

AnimNodeI * AnimNodeI::Speed( Anim_float speed )
{
    this->_speed = speed;
    return this;
}

AnimNodeI * AnimNodeI::Speed_inf()
{
    this->_speed = std::numeric_limits< Anim_float >::infinity();
    return this;
}

bool AnimNodeI::IsInTarget()
{
    return this->Distance() == Anim_float( 0.0 ) && this->TargetStabilized();
}

}
