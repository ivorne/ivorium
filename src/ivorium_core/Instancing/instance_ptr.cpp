#include "instance_ptr.hpp"
#include "Instance.hpp"

namespace iv
{

void instance_ptr_interface::addSelfToSC( Instance * inst )
{
    inst->instance_pointers.insert( this );
}
void instance_ptr_interface::removeSelfFromSC( Instance * inst )
{
    inst->instance_pointers.erase( this );
}

instance_ptr::instance_ptr( Instance * instance ) :
    inst( instance )
{
    if( this->inst )
        this->addSelfToSC( this->inst );
}

instance_ptr::instance_ptr( instance_ptr const & other ) :
    inst( other.inst )
{
    if( this->inst )
        this->addSelfToSC( this->inst );
}

instance_ptr::~instance_ptr( )
{
    if( this->inst )
        this->removeSelfFromSC( this->inst );
}

instance_ptr & instance_ptr::operator=( instance_ptr const & other )
{
    if( this->inst == other.inst )
        return *this;
    
    if( this->inst )
        this->removeSelfFromSC( this->inst );
    
    this->inst = other.inst;
    
    if( this->inst )
        this->addSelfToSC( this->inst );
    
    return *this;
}

instance_ptr & instance_ptr::operator=( Instance * instance )
{
    if( this->inst == instance )
        return *this;
    
    if( this->inst )
        this->removeSelfFromSC( this->inst );
    
    this->inst = instance;
    
    if( this->inst )
        this->addSelfToSC( this->inst );
    
    return *this;
}

bool instance_ptr::operator==( instance_ptr const & other )
{
    return this->inst == other.inst;
}

bool instance_ptr::operator!=( instance_ptr const & other )
{
    return this->inst != other.inst;
}

bool instance_ptr::operator<( instance_ptr const & other )
{
    return this->inst < other.inst;
}

bool instance_ptr::operator>( instance_ptr const & other )
{
    return this->inst > other.inst;
}

bool instance_ptr::operator<=( instance_ptr const & other )
{
    return this->inst <= other.inst;
}

bool instance_ptr::operator>=( instance_ptr const & other )
{
    return this->inst >= other.inst;
}

Instance * instance_ptr::get() const
{
    return this->inst;
}

Instance & instance_ptr::operator*() const
{
    return *this->inst;
}

Instance * instance_ptr::operator->() const
{
    return this->inst;
}

instance_ptr::operator bool() const
{
    return this->inst;
}

void instance_ptr::invalidate( Instance * inst )
{
    this->inst = nullptr;
}


callback_instance_ptr & callback_instance_ptr::operator=( callback_instance_ptr const & other )
{
    ( instance_ptr & )( *this ) = ( instance_ptr const & )other;
    if( this->_change_callback )
        this->_change_callback();    
    return *this;
}

callback_instance_ptr & callback_instance_ptr::operator=( Instance * instance )
{
    ( instance_ptr & )( *this ) = instance;
    if( this->_change_callback )
        this->_change_callback();
    return *this;
}

void callback_instance_ptr::set_change_callback( std::function< void() > const & cb )
{
    this->_change_callback = cb;
}

void callback_instance_ptr::invalidate( Instance * inst )
{
    instance_ptr::invalidate( inst );
    if( this->_change_callback )
        this->_change_callback();
}

}
