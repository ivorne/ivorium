#include "Instance.hpp"

#include "instance_ptr.hpp"
#include "DebugInstanceListener.hpp"
#include "SystemContainer.hpp"

#include "../DebugView/TextDebugView.hpp"

namespace iv
{

Instance::Instance( const SystemContainer * sc ) :
    sc( sc ),
    sc_dup( nullptr ),
    os( nullptr ),
    _instance_name(),
    _parent( nullptr ),
    _children(),
    _root_client( nullptr ),
    _clients(),
    _logging_cnt( 0 ),
    instance_pointers()
{
}

Instance::~Instance()
{
    // unregister
    if( this->_root_client )
    {
        // 
        if( this->getOS() )
            this->getOS()->unregister_instance( this );
        
        //
        if( this->getOS() )
            this->getOS()->debug_listeners().foreach(
                [ & ]( DebugInstanceListener * const & listener )
                {
                    listener->InstanceDestroyed( this );
                }
            );
    }
    
    // remove from parent and children
    if( this->_parent )
        this->_parent->_children.erase( this );
    for( Instance * inst : this->_children )
        inst->_parent = nullptr;
    
    // delete duplicated system container
    if( this->sc_dup )
        delete this->sc_dup;
}

unsigned Instance::frame_id() const
{
    return this->getSystemContainer()->frame_id();
}

void Instance::instance_finalize( std::string const & inst_name, ClientMarker const * marker )
{
    if( this->_root_client )
        if( this->getOS() )
            this->getOS()->log( SRC_INFO, Defs::Log::Warning, "There already is a root ClientMarker in this Instance (Instance was probably already finalized)." );
        
    // 
    this->_instance_name = inst_name;
    this->_root_client = marker;
    
    //-- finalize creation ---
    if( this->getOS() )
        this->getOS()->register_instance( this );
    
    if( this->getOS() ) 
        this->getOS()->debug_listeners().foreach(
            [ & ]( DebugInstanceListener * const & listener )
            {
                listener->InstanceCreated( this );
            }
        );
}

void Instance::instance_parent( Instance * parent )
{
    if( this->_parent )
        this->_parent->_children.erase( this );
    
    this->_parent = parent;
    
    if( this->_parent )
        this->_parent->_children.insert( this );
}

void Instance::debug_print_clients( TextDebugView * view )
{
    ClientMarker const * cm = this->Debug_RootClient();
    if( cm )
        cm->print_status_with_related( view );
}

SystemContainer const * Instance::getSystemContainer() const
{
    return this->sc;
}

SystemContainer * Instance::duplicateSystemContainer()
{
    if( !this->sc_dup )
        this->sc_dup = new SystemContainer( this->sc );
    
    return this->sc_dup;
}

InstanceSystem * Instance::getOS( )
{
    if( this->os )
    {
        return this->os;
    }
    else
    {
        this->os = sc->getSystem< InstanceSystem >( );
        return this->os;
    }
}

void Instance::client_register( ClientMarker const * cm )
{
    this->_clients.insert( cm );
    
    if( this->getOS() )
        this->getOS()->debug_listeners().foreach(
            [ & ]( DebugInstanceListener * const & listener )
            {
                listener->ClientCreated( cm );
            }
        );
}

void Instance::client_unregister( ClientMarker const * cm )
{
    this->_clients.erase( cm );
    
    if( this->getOS() )
        this->getOS()->debug_listeners().foreach(
            [ & ]( DebugInstanceListener * const & listener )
            {
                listener->ClientDestroyed( cm );
            }
        );
}

void Instance::client_log( ClientMarker const * cm, SrcInfo const & info, LogId id, std::string const & message )
{
    if( this->getOS() )
        this->getOS()->ClientLog( cm, info, id, message );
    
}

bool Instance::client_log_enabled( ClientMarker const * cm, LogId id )
{
    if( this->getOS() )
        return this->getOS()->ClientLogEnabled( cm, id );
    
    return false;
}

std::string const & Instance::instance_name()
{
    return this->_instance_name;
}

Instance * Instance::Debug_Parent()
{
    return this->_parent;
}

std::unordered_set< Instance * > const & Instance::Debug_Children()
{
    return this->_children;
}

ClientMarker const * Instance::Debug_RootClient()
{
    return this->_root_client;
}

std::unordered_set< ClientMarker const * > const & Instance::Debug_Clients()
{
    return this->_clients;
}

}
