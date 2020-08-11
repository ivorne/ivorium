#include "Elem.hpp"
#include "Camera.hpp"
#include "ElementSystem.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

Elem::Elem( Instance * inst ) :
    InputNode( inst ),
    cm( inst, this, "Elem", ClientMarker::Status() ),
    attr_enabled( &this->cm, true ),
    modelTransform( &this->cm, float4x4( 1 ) ),
    scissor( &this->cm ),
    inst( inst ),
    _first_pass_frame_id( 0 ),
    _second_pass_frame_id( 0 ),
    _quiet( false ),
    _elem_parent( nullptr )
{
    this->cm.inherits( this->InputNode::cm );
    
    auto es = this->instance()->getSystem< ElementSystem >();
    if( es )
        es->elem_register( this );
}

Elem::~Elem()
{
    if( this->_elem_parent )
        this->_elem_parent->elem_childDisconnect( this );
    
    auto es = this->instance()->getSystem< ElementSystem >();
    if( es )
        es->elem_unregister( this );
}


Elem * Elem::elem_getParent()
{
    return this->_elem_parent;
}

Camera * Elem::elem_getRoot()
{
    if( auto parent = this->elem_getParent() )
        return parent->elem_getRoot();
    else
        return nullptr;
}

void Elem::elem_setParent( Elem * parent )
{
    this->_elem_parent = parent;
}

Instance * Elem::instance() const
{
    return this->inst;
}

void Elem::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Elem" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "enabled", this->attr_enabled.Get() );
    row.Column( "modelTransform", this->modelTransform.Get() );
    row.Column( "scissor", this->scissor.Get().enabled );
}

void Elem::first_pass( ElementRenderer * er )
{
    if( this->attr_enabled.dirty() )
    {
        this->attr_enabled.clear_dirty();
        this->InputNode::inputEnabled( this->attr_enabled.Get() );
    }
    
    if( !this->attr_enabled.Get() )
        return;
    
    this->_first_pass_frame_id = this->instance()->frame_id();
    
    this->first_pass_impl( er );
}

void Elem::second_pass( ElementRenderer * er )
{
    if( !this->attr_enabled.Get() )
        return;
    
    if( this->instance()->frame_id() == this->second_pass_frame_id() )
        this->cm.warning( SRC_INFO, "Element already had second pass called on itself this frame (order of second passes is wrong)." );
    
    this->_second_pass_frame_id = this->instance()->frame_id();
    
    er->DequeueSecondPass( this );
    
    this->second_pass_impl( er );
}

float3 Elem::FromLocalSpaceToScreenSpace( float3 local_space )
{
    // camera check
    Camera * camera = this->elem_getRoot();
    if( !camera )
        return float3( 0, 0, 0 );

    // transform
    float4x4 model = this->modelTransform.Get();
    return camera->FromLocalSpaceToViewportSpace( model, local_space );
}

float3 Elem::FromScreenSpaceToLocalSpace( float3 screen_space )
{
    // camera check
    Camera * camera = this->elem_getRoot();
    if( !camera )
        return float3( 0, 0, 0 );

    // transform
    float4x4 model = this->modelTransform.Get();
    return camera->FromViewportSpaceToLocalSpace( model, screen_space );
}

float2 Elem::FromScreenPlaneToLocalPlane( float2 screen_space )
{
    // camera check
    Camera * camera = this->elem_getRoot();
    if( !camera )
        return float2( 0, 0 );
    
    // transform
    float4x4 model = this->modelTransform.Get();
    return camera->FromViewportPlaneToLocalPlane( model, screen_space );
}

Elem * Elem::enabled( bool val )
{
    this->attr_enabled.Set( val );
    return this;
}

unsigned Elem::first_pass_frame_id()
{
    return this->_first_pass_frame_id;
}

unsigned Elem::second_pass_frame_id()
{
    return this->_second_pass_frame_id;
}

void Elem::quiet( bool val )
{
    this->_quiet = val;
}

bool Elem::quiet() const
{
    return this->_quiet;
}

void Elem::Add_InputNode( InputNode * node )
{
    // disconnect node from previous parent
    if( node->input_getParent() )
        node->input_getParent()->input_childDisconnect( node );
    
    //
    this->_input_children.push_back( node );
    node->input_setParent( this );
}

void Elem::Remove_InputNode( InputNode * node )
{
    this->input_childDisconnect( node );
}

void Elem::input_childDisconnect( InputNode * node )
{
    for( size_t i = 0; i < this->_input_children.size(); i++ )
        if( this->_input_children[ i ] == node )
        {
            this->_input_children[ i ]->input_setParent( nullptr );
            this->_input_children.erase( this->_input_children.begin() + i );
            break;
        }
}

void Elem::input_eachChild( std::function< void( InputNode * ) > const & f )
{
    for( auto it = this->_input_children.rbegin(); it != this->_input_children.rend(); ++it )
        f( *it );
}

//=================================================
Pickable::Pickable( Elem * elem ) :
    cm( elem->instance(), this, "Pickable" ),
    _elem( elem )
{
}

Elem * Pickable::elem()
{
    return this->_elem;
}

Elem const * Pickable::elem() const
{
    return this->_elem;
}

}
