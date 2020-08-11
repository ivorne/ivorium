#pragma once

#include "InputEvent.hpp"
#include "InputNode.hpp"

#include "InputBindingSystem/InputBindingQuery.hpp"
#include "InputBindingSystem/InputId.hpp"

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    Implementation of input_on_pressed and input_on_released methods is maybe not fully polished.
    I am not sure how it should behave in all corner cases so lets just keep it simple for now (it is probably sufficient implementation) and tweak it if any problems arise.
*/
class Key_InputNode : public InputNode, public InputEvent
{
public:
using InputEvent::instance;
ClientMarker cm;
    
                            Key_InputNode( Instance * inst, InputId val = InputId() );
                            ~Key_InputNode();
    
    void                    status( iv::TableDebugView * view );
    
    void                    input_id( InputId );
    InputId                 input_id();
    
    /**
        Defaults to false.
        If this is true, the event will only accept one key at time.
        So if two or more keys are pressed for the same InputId simultaneously, only one will be accepted
            by this event and the rest will fall through to events that are below this one in InputNode hierarchy.
        This does not have much use in KeyInputEvent, it is kept here to be consistent with behavior of Position_InputNode.
    */
    void                    fallthrough_enabled( bool );
    bool                    fallthrough_enabled();
    
    //------------------
    /**
        Activation manually interrupted from game code.
        Should work the same as non-real release.
        Called when InputEvent implementor changes meaning, so that field_release is not triggered if the event was pressed when the meaning of the InputEvent was different. 
    */
    void                    interruptActivation();
    
protected:
    virtual void            input_process( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace ) override;
    virtual bool            input_trigger_process( InputRoot * root, Input::DeviceKey key ) override;
    
private:
    InputBindingQuery       ibq;
    InputId                 _input_id;
    
    bool                    _fallthrough;
    std::optional< Input::DeviceKey > _active;
};

}
