#include "InputBindingQuery.hpp"

namespace iv
{

InputBindingQuery::InputBindingQuery( Instance * inst ) :
    cm( inst, this, "InputBindingQuery" ),
    inst( inst ),
    ies( inst->getSystemContainer()->getSystem< InputBindingSystem >() )
{
}

InputBindingQuery::~InputBindingQuery()
{
}

Instance * InputBindingQuery::instance() const
{
    return this->inst;
}

bool InputBindingQuery::IsBound( InputId inputId, Input::DeviceKey device_key )
{
    if( !this->ies )
        return false;
    
    return this->ies->IsBound( inputId, device_key.first, device_key.second );
}

bool InputBindingQuery::IsHoverBound( InputId inputId, Input::DeviceKey device_key )
{
    if( !this->ies )
        return false;
    
    auto result = this->ies->IsHoverBound( inputId, device_key.first, device_key.second );
    return result;
}

}
