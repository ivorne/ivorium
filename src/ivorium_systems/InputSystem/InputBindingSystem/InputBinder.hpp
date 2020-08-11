#pragma once

#include "InputId.hpp"

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class Instance;
class InputBindingSystem;

class InputBinder
{
public:
ClientMarker cm;
                        InputBinder( Instance * inst );
    Instance *          instance();
    
    void                BindKey( InputId inputId, Input::Key key, int device_id = -1 );
    void                UnbindKey( InputId inputId, Input::Key key, int device_id = -1 );
    
    void                BindHoverKey( InputId inputId, Input::Key key, int device_id = -1 );
    void                UnbindHoverKey( InputId inputId, Input::Key key, int device_id = -1 );
    
private:
    Instance * inst;
    InputBindingSystem * ies;
};

}
