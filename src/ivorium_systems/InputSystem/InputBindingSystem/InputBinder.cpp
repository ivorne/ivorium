#include "InputBinder.hpp"
#include "InputBindingSystem.hpp"

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

InputBinder::InputBinder( Instance * inst ) :
    cm( inst, this, "InputBinder" ),
    inst( inst ),
    ies( inst->getSystemContainer()->getSystem< InputBindingSystem >() )
{
}

Instance * InputBinder::instance()
{
    return this->inst;
}

void InputBinder::BindKey( InputId inputId, Input::Key key, int device_id )
{
    if( !this->ies )
        return;
    
    this->ies->BindKey( inputId, key, device_id );
}

void InputBinder::UnbindKey( InputId inputId, Input::Key key, int device_id )
{
    if( !this->ies )
        return;
    
    this->ies->UnbindKey( inputId, key, device_id );
}

void InputBinder::BindHoverKey( InputId inputId, Input::Key key, int device_id )
{
    if( !this->ies )
        return;
    
    this->ies->BindHoverKey( inputId, key, device_id );
}

void InputBinder::UnbindHoverKey( InputId inputId, Input::Key key, int device_id )
{
    if( !this->ies )
        return;
    
    this->ies->UnbindHoverKey( inputId, key, device_id );
}

}
