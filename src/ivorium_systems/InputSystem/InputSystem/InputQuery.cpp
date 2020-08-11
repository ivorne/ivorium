#include "InputQuery.hpp"

namespace iv
{

InputQuery::InputQuery( iv::Instance * inst ) :
    inst( inst ),
    is( inst->getSystem< InputSystem >() )
{
}

iv::Instance * InputQuery::instance() const
{
    return this->inst;
}

int2 InputQuery::input_position( Input::Key key, int device_id )
{
    if( !this->is )
        return int2( 0, 0 );
    return this->is->input_position( key, device_id );
}

float InputQuery::input_value( Input::Key key, int device_id )
{
    if( !this->is )
        return 0.0f;
    return this->is->input_value( key, device_id );
}

unsigned InputQuery::input_character()
{
    if( !this->is )
        return 0;
    return this->is->input_character();
}

}
