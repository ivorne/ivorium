#include "SystemContainer.hpp"

#include "../Basics/TextOutput.hpp"
#include "../Basics/utils.hpp"

#include "InstanceSystem.hpp"
#include "../Defs.hpp"
#include "../Basics/StringIO_defs.hpp"

namespace iv
{

//--------------------- System -----------------------------------------------------
System::System( SystemContainer * sc ) :
    sc( sc ),
    _refcounters( 0 )
{
}

SystemContainer * System::system_container() const
{
    return this->sc;
}

void System::retain()
{
    this->_refcounters++;
}

void System::release()
{
    this->_refcounters--;
    if( this->_refcounters == 0 )
        delete this;
}

bool System::log_process_enabled( LogId id ) const
{
    auto is = this->sc->getSystem< InstanceSystem >();
    if( !is )
        return false;
    
    return is->SystemLogEnabled( this, id );
}

void System::log_process( SrcInfo const & info, LogId id, std::string const & message ) const
{
    auto is = this->sc->getSystem< InstanceSystem >();
    if( !is )
        return;
    
    is->SystemLog( this, info, id, message );
}

//--------------------- SystemContainer -----------------------------------------------------
SystemContainer::SystemContainer() :
    systems(),
    own_systems(),
    _frame_id( 0 )
{
}

SystemContainer::SystemContainer( SystemContainer const * sc ) :
    systems(),
    own_systems(),
    _frame_id( 0 )
{
    // copy pointers to systems
    this->systems = sc->systems;
    
    for( auto & [ type, system ] : this->systems )
        system->retain();
}

SystemContainer::~SystemContainer()
{
    // delete all systems
    for( auto & [ type, system ] : this->systems )
        system->release();
}

void SystemContainer::nextFrame()
{
    //if( this->systems.count( typeid( InstanceSystem ) ) )
    //{
    //  auto is = this->systems.at( typeid( InstanceSystem ) );
    //  is->log( SRC_INFO, ::ivorium::Defs::Log::NewFrame, "==========================================================================" );
    //}
    
    this->_frame_id++;
    
    if( this->systems.count( typeid( InstanceSystem ) ) )
    {
        auto is = this->systems.at( typeid( InstanceSystem ) );
        is->log( SRC_INFO, ::iv::Defs::Log::NewFrame, "============================ <frame ", this->_frame_id, "> =================================" );
    }
}

unsigned SystemContainer::frame_id() const
{
    return this->_frame_id;
}

System * SystemContainer::getSystem( std::type_index type ) const
{
    // check if system exists
    if( !this->systems.count( type ) )
        return nullptr;
    
    // return system
    return this->systems.at( type );
}

std::unordered_map< std::type_index /*type*/, System * > const & SystemContainer::debug_GetSystems() const
{
    return this->systems;
}

bool SystemContainer::flushSystems()
{
    bool changed = true;
    while( changed )
    {
        changed = false;
        
        // flush own systems
        for( System * system : this->own_systems )
            changed = changed || system->flushSystem();
    }
    
    return changed;
}

void SystemContainer::debug_print( TextDebugView * view ) const
{
    view->out() << "============================== SystemContainer ==============================" << std::endl;
    
    for( auto & [ type, system ] : this->systems )
    {
        view->out() << system->debug_name() << ":" << std::endl;
        view->prefix_push( "  " );
        system->status( view );
        view->out() << std::endl;
        view->prefix_pop();
    }
    
    view->out() << "#############################################################################" << std::endl;
}

}
