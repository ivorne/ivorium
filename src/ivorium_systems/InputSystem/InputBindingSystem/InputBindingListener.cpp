#include "InputBindingListener.hpp"
#include "InputBindingSystem.hpp"

namespace iv
{

InputBindingListener::InputBindingListener( Instance * inst ) :
    inst( inst ),
    ibs( inst->getSystem< InputBindingSystem >() )
{
    if( !this->ibs )
        return;
    
    this->ibs->register_listener( this );
}

InputBindingListener::~InputBindingListener()
{
    if( !this->ibs )
        return;
    
    this->ibs->unregister_listener( this );
}

Instance * InputBindingListener::instance() const
{
    return this->inst;
}

}
