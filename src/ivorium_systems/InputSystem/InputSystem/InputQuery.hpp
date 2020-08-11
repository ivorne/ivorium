#pragma once

#include "InputSystem.hpp"

namespace iv
{

class InputQuery
{
public:
                            InputQuery( iv::Instance * inst );
    iv::Instance *          instance() const;
    
    int2                    input_position( Input::Key key, int device_id );
    float                   input_value( Input::Key key, int device_id );
    unsigned                input_character();
    
private:
    Instance * inst;
    InputSystem * is;
};

}
