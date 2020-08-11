#pragma once

#include "InputBindingSystem/InputBindingQuery.hpp"
#include "InputSystem/InputQuery.hpp"
#include "InputNode.hpp"
#include "InputEvent.hpp"
#include "../TimeSystem/FrameUpdateClient.hpp"

namespace iv
{

/**
    NOTE  - possible optimization: Disable KeyInputEvent if cursor is not on this item (on_picking_left was called).
        This needs testing, if it will realy be faster (removing and adding KeyInputEvent to InputEventSystem takes some performance too).
*/
class Position_InputNode : public InputNode, public InputEvent, private FrameUpdateClient
{
public:
    class Tester
    {
    public:
        /**
            \returns True if the input is inside tested area.
        */
        virtual bool            position_test( int2 input_pos ) = 0;
    };
    
public:
ClientMarker cm;
using InputNode::instance;
    
    /**
        Positional_input_name označuje nějaké binární tlačítko, kte kterému je připojena informace o pozici (víceméně tlačítka myši).
        Enable keeps_press_in_offspace for drag and drop items so they do not lose activation by mistake.
    */
                            Position_InputNode( Instance * inst, Tester * tester, bool keeps_press_in_offspace );
                            ~Position_InputNode();
    void                    status( iv::TableDebugView * view );

//------------------------- configuration ---------------------------
    void                    input_id( InputId );
    InputId                 input_id();

    
    /**
        Defaults to false.
        If this is true, the event will only accept one key at time.
        So if two or more keys are pressed for the same InputId simultaneously, only one will be accepted
            by this event and the rest will fall through to events that are below this one in InputNode hierarchy.
    */
    void                    fallthrough_enabled( bool );
    bool                    fallthrough_enabled();

//------------------------- query --------------------------------------------
    PrivValAttr< bool >     attr_hover;

    int2                    input_position_screen();
    
    //-------------
    /**
        Activation manually interrupted from game code.
        Should work the same as non-real release.
        Called when InputEvent implementor changes meaning, so that field_release is not triggered if the event was pressed when the meaning of the InputEvent was different. 
    */
    void                    interruptActivation();

private:
    // FrameUpdateClient
    virtual void            frame_update() override;
    
private:
    // InputNode
    virtual void            input_process( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace ) override;
    virtual bool            input_trigger_process( InputRoot * root, Input::DeviceKey key ) override;
    
    //
    void                    input_process_hover( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace );
    void                    setup_frame_update();
    void                    reserve_hover_keys( InputRoot * root, bool reserve );
    
private:
    Tester * tester;
    bool keeps_press_in_offspace;

    InputBindingQuery ibq;
    InputQuery iq;
    InputId _input_id;
    
    bool _fallthrough;
    
    std::optional< Input::DeviceKey > _active;
    std::optional< Input::DeviceKey > _durated;
    
    std::unordered_map< Input::DeviceKey, bool /*hit_last*/, Input::DeviceKey_Hash > _watched;
    
    std::optional< Input::DeviceKey > _last;
};

}
