#include "InstanceSystem.hpp"
#include "Instance.hpp"
#include "instance_ptr.hpp"
#include <iostream>
#include <cstring>
#include "../DebugView/TextDebugView.hpp"
#include "../DebugView/TreeDebugView.hpp"
#include "DebugInstanceListener.hpp"

namespace iv
{

InstanceSystem::InstanceSystem( SystemContainer * sc ) :
    System( sc ),
    last_check_frame_id( 0 )
{
}

bool InstanceSystem::flushSystem()
{
    unsigned frame_id = this->system_container()->frame_id();
    if( this->last_check_frame_id != frame_id )
    {
        this->last_check_frame_id = frame_id;
        
        this->log( SRC_INFO, iv::Defs::Log::ConsistencyChecks_Frame, "ConsistencyChecks_Frame." );
        if( this->log_enabled( iv::Defs::Log::ConsistencyChecks_Frame ) )
        {
            for( Instance * instance : this->_instances )
            {
                //
                if( !instance->Debug_RootClient() )
                    this->log( SRC_INFO, iv::Defs::Log::ConsistencyChecks_Frame, "Instance without root ClientMarker '",instance->instance_name(),"' found. It did not have Instance::instance_finalize called on itself." );
                
                //
                for( ClientMarker const * client : instance->Debug_Clients() )
                {
                    bool has_parent = client == instance->Debug_RootClient() || client->inheritance_child() || client->ownership_parent();
                    if( !has_parent )
                        this->log( SRC_INFO, iv::Defs::Log::ConsistencyChecks_Frame, "Orphaned ClientMarker ", client, " found. Instance root is ",instance->Debug_RootClient(),"." );
                }
            }
        }
    }
    
    return false;
}

void InstanceSystem::DebugPrintInstance( TreeDebugView * tree, Instance * inst )
{
    SS title;
    title << inst;
    if( inst->instance_name().size() )
        title << " \"" << inst->instance_name() << "\"";
    tree->Push( title<<SS::c_str() );
    
    inst->debug_print_clients( tree );
    
    for( Instance * child : inst->Debug_Children() )
    {
        if( child->getSystem< InstanceSystem >() == inst->getSystem< InstanceSystem >() )
            this->DebugPrintInstance( tree, child );
        else
            tree->out() << "-> instance in different InstanceSystem" << std::endl;
    }
    
    tree->Pop();
}

void InstanceSystem::status( TextDebugView * view )
{
    TreeDebugView tree( view->context() );
    std::unordered_set< Instance * > visited;
    for( Instance * inst : this->Debug_Instances() )
        if( !inst->Debug_Parent() )
            this->DebugPrintInstance( &tree, inst );
    tree.Write( TreeDebugView::Style::BoldFramesWeakLinks, view );
}

std::unordered_set< Instance * > const & InstanceSystem::Debug_Instances()
{
    return this->_instances;
}

volatile_set< DebugInstanceListener * > & InstanceSystem::debug_listeners()
{
    return this->_debug_listeners;
}
    
void InstanceSystem::register_instance( Instance * inst )
{
    this->_instances.insert( inst );
}

void InstanceSystem::unregister_instance( Instance * inst )
{
    this->_instances.erase( inst );
}

void InstanceSystem::AddDebugListener( DebugInstanceListener * listener )
{
    this->_debug_listeners.insert( listener );
}

void InstanceSystem::RemoveDebugListener( DebugInstanceListener * listener )
{
    this->_debug_listeners.erase( listener );
}

void InstanceSystem::ClientLog( ClientMarker const * cm, SrcInfo const & info, LogId id, std::string const & message )
{
    this->_debug_listeners.foreach(
        [ & ]( DebugInstanceListener * const & listener )
        {
            listener->ClientLog( cm, info, id, message );
        }
    );
}

bool InstanceSystem::ClientLogEnabled( ClientMarker const * cm, LogId id )
{
    bool enabled = false;
    
    this->_debug_listeners.foreach(
        [ & ]( DebugInstanceListener * const & listener )
        {
            enabled = enabled || listener->ClientLogEnabled( cm, id );
        }
    );
    
    return enabled;
}

void InstanceSystem::SystemLog( System const * system, SrcInfo const & info, LogId id, std::string const & message )
{
    this->_debug_listeners.foreach(
        [ & ]( DebugInstanceListener * const & listener )
        {
            listener->SystemLog( system, info, id, message );
        }
    );
}

bool InstanceSystem::SystemLogEnabled( System const * system, LogId id )
{
    bool enabled = false;
    
    this->_debug_listeners.foreach(
        [ & ]( DebugInstanceListener * const & listener )
        {
            enabled = enabled || listener->SystemLogEnabled( system, id );
        }
    );
    
    return enabled;
}


}
