#include "Lambda_Connector.hpp"
#include "../../Defs.hpp"

namespace iv
{

Lambda_Connector::Lambda_Connector( Instance * inst ) :
    AnimConnector( inst ),
    cm( inst, this, "Lambda_Connector" ),
    _retarget( true )
{
    this->cm.inherits( this->AnimConnector::cm );
}

Lambda_Connector::~Lambda_Connector()
{
    for( AnimNodeI * node : this->_parents )
        node->anim_removeChild( this );
    
    for( AnimNodeI * node : this->_children )
        node->anim_unsetParent( this );
}

void Lambda_Connector::anim_eachChild( std::function< void( AnimNodeI * ) > const & f )
{
    for( AnimNodeI * node : this->_children )
        f( node );
}

void Lambda_Connector::anim_childDisconnect( AnimNodeI * node )
{
    this->_children.erase( node );
    node->anim_unsetParent( this );
    this->Activate();
    this->_retarget = true;
}

void Lambda_Connector::anim_eachParent( std::function< void( AnimNodeI * ) > const & f )
{
    for( AnimNodeI * node : this->_parents )
        f( node );
}

void Lambda_Connector::anim_parentDisconnect( AnimNodeI * node )
{
    this->_parents.erase( node );
    node->anim_removeChild( this );
    this->Activate();
    this->_retarget = true;
}

Lambda_Connector * Lambda_Connector::AddParent( AnimNodeI * node )
{
    this->_parents.insert( node );
    node->anim_addChild( this );
    this->Activate();
    this->_retarget = true;
    return this;
}

Lambda_Connector * Lambda_Connector::AddChild( AnimNodeI * node )
{
    this->_children.insert( node );
    node->anim_setParent( this );
    this->Activate();
    this->_retarget = true;
    return this;
}

void Lambda_Connector::Retargeting( std::function< void( bool warping ) > const & val )
{
    this->_retargeting = val;
    this->Activate();
    this->_retarget = true;
}

void Lambda_Connector::UpdatePass_Down()
{
    // decide if we need to retarget
    bool retarget = this->_retarget || this->parentChanged_Get();
    this->_retarget = false;
    this->parentChanged_Clear();
    
    // compute update distances
    Anim_float distance = 0.0f;
    for( AnimNodeI * parent : this->_parents )
        distance = std::max( distance, parent->Step() );
    
    //
    this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Down | Retarget=", retarget, ", step=", distance, "." );
    
    // retarget
    if( retarget )
        this->Retarget( distance == std::numeric_limits< Anim_float >::infinity() );
    
    // request updates on children
    for( AnimNodeI * child : this->_children )
        child->Connector_RequestStep( distance );
}

void Lambda_Connector::UpdatePass_Up()
{
    // compute distance from children
    if( this->childArrived_Get() )
    {
        this->childArrived_Clear();
        this->_retarget = true;
    }
    
    // aggregate distance
    Anim_float distance = 0.0f;
    for( AnimNodeI * child : this->_children )
        distance = std::max( distance, child->Distance() );
    
    if( this->_retarget )
        distance = std::max( distance, std::numeric_limits< Anim_float >::epsilon() );
    
    //
    this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Up | distance=", distance, "." );
    
    // set distance to parents
    for( AnimNodeI * parent : this->_parents )
        parent->Connector_UpdateDistance( distance );
}

void Lambda_Connector::Retarget( bool warping )
{
    if( warping )
        this->cm.log( SRC_INFO, iv::Defs::Log::Animation_Events, "Retarget (warping)." );
    else
        this->cm.log( SRC_INFO, iv::Defs::Log::Animation_Events, "Retarget." );
    
    this->Activate();
    this->_retargeting( warping );
    
    if( this->cm.log_enabled( iv::Defs::Log::Animation_Events ) )
        for( AnimNodeI * child : this->_children )
            this->cm.log( SRC_INFO, iv::Defs::Log::Animation_Events, "    Child ", child->cm, " (", child->label(), ") -> ", child->StringIO_GetTarget( &this->cm ), " (speed=",child->Speed(),")." );
}

}
