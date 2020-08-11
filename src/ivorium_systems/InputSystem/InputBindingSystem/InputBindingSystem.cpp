#include "InputBindingSystem.hpp"
#include "InputBindingQuery.hpp"
#include "InputBindingListener.hpp"

namespace iv
{

InputBindingSystem::InputBindingSystem( SystemContainer * sc ) :
    System( sc ),
    binds()
{
}

void InputBindingSystem::status( TextDebugView * view )
{
    auto & out = view->out();
    
    for( auto & [ input_id, binds ] : this->binds )
    {
        out << "InputId::" << StringIO_Write( input_id, view->context() ) << ": ";
        
        bool first = true;
        for( Input::Key const & key : binds.all )
        {
            if( !first )
                out << ", ";
            first = false;
            
            out << "( Key::" << StringIO_Write( key, view->context() ) << ", -1 )";
        }
        
        for( Input::DeviceKey const & device_key : binds.device )
        {
            if( !first )
                out << ", ";
            first = false;
            
            out << "( Key::" << StringIO_Write( device_key.first, view->context() ) << ", "<<  device_key.second <<" )";
        }
        
        if( first )
            out << "<unbound>";
        
        out << std::endl;
    }
    
    
    for( auto & [ input_id, binds ] : this->hover_binds )
    {
        out << "InputId::" << StringIO_Write( input_id, view->context() ) << " (hover): ";
        
        bool first = true;
        for( Input::Key const & key : binds.all )
        {
            if( !first )
                out << ", ";
            first = false;
            
            out << "( Key::" << StringIO_Write( key, view->context() ) << ", -1 )";
        }
        
        for( Input::DeviceKey const & device_key : binds.device )
        {
            if( !first )
                out << ", ";
            first = false;
            
            out << "( Key::" << StringIO_Write( device_key.first, view->context() ) << ", "<<  device_key.second <<" )";
        }
        
        if( first )
            out << "<unbound>";
        
        out << std::endl;
    }
}

void InputBindingSystem::register_listener( InputBindingListener * lst )
{
    this->listeners.insert( lst );
}

void InputBindingSystem::unregister_listener( InputBindingListener * lst )
{
    this->listeners.erase( lst );
}

void InputBindingSystem::binding_changed()
{
    this->listeners.foreach( 
        [&]( InputBindingListener * lst )
        {
            lst->on_binding_changed();
        }
    );
}

void InputBindingSystem::BindKey( InputId inputId, Input::Key key, int device_index )
{
    if( device_index < 0 )
        this->binds[ inputId ].all.insert( key );
    else
        this->binds[ inputId ].device.insert( std::pair( key, device_index ) );
    
    this->binding_changed();
}

void InputBindingSystem::UnbindKey( InputId inputId, Input::Key key, int device_index )
{
    auto bind = this->binds[ inputId ];
    if( device_index < 0 )
        bind.all.erase( key );
    else
        bind.device.erase( std::pair( key, device_index ) );
        
    if( bind.all.empty() && bind.device.empty() )
        this->binds.erase( inputId );
    
    this->binding_changed();
}

bool InputBindingSystem::IsBound( InputId inputId, Input::Key key, int device_index )
{
    auto it = this->binds.find( inputId );
    if( it == this->binds.end() )
        return false;
    
    if( it->second.all.count( key ) )
        return true;
    
    if( it->second.device.count( std::pair( key, device_index ) ) )
        return true;
    
    return false;
}


void InputBindingSystem::BindHoverKey( InputId inputId, Input::Key key, int device_index )
{
    if( device_index < 0 )
        this->hover_binds[ inputId ].all.insert( key );
    else
        this->hover_binds[ inputId ].device.insert( std::pair( key, device_index ) );
    
    this->binding_changed();
}

void InputBindingSystem::UnbindHoverKey( InputId inputId, Input::Key key, int device_index )
{
    auto bind = this->hover_binds[ inputId ];
    if( device_index < 0 )
        bind.all.erase( key );
    else
        bind.device.erase( std::pair( key, device_index ) );
        
    if( bind.all.empty() && bind.device.empty() )
        this->hover_binds.erase( inputId );
    
    this->binding_changed();
}

bool InputBindingSystem::IsHoverBound( InputId inputId, Input::Key key, int device_index )
{
    auto it = this->hover_binds.find( inputId );
    if( it == this->hover_binds.end() )
        return false;
    
    if( it->second.all.count( key ) )
        return true;
    
    if( it->second.device.count( std::pair( key, device_index ) ) )
        return true;
    
    return false;
}

}
