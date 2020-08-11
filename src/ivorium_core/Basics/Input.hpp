#pragma once

#include <string>

#include "glm_alias.hpp"
#include "hash.hpp"
#include "StringIO_Table.hpp"

namespace iv
{


class Input
{
public:
    enum class Key
    {
        None        = 0,
        
        Character,      ///< Key that can be interpreted as a character was pressed somewhere. Code of the character can be retrieved from InputSource. This will be only sent as a Type::Trigger.
        
        //---------------- KEYBOARD -------------------------
        KEYBOARD_BEGIN,
        Space = KEYBOARD_BEGIN,
        Apostrophe,
        Comma,
        Minus,
        Period,
        Slash,
        Num_0,
        Num_1,
        Num_2,
        Num_3,
        Num_4,
        Num_5,
        Num_6,
        Num_7,
        Num_8,
        Num_9,
        Semicolon,
        Equal,
        Char_A,
        Char_B,
        Char_C,
        Char_D,
        Char_E,
        Char_F,
        Char_G,
        Char_H,
        Char_I,
        Char_J,
        Char_K,
        Char_L,
        Char_M,
        Char_N,
        Char_O,
        Char_P,
        Char_Q,
        Char_R,
        Char_S,
        Char_T,
        Char_U,
        Char_V,
        Char_W,
        Char_X,
        Char_Y,
        Char_Z,
        Left_Bracket,
        Backslash,
        Right_Bracket,
        Grave_Accent,
        World_1,
        World_2,
        Escape,
        Enter,
        Tab,
        Backspace,
        Insert,
        Delete,
        Right,
        Left,
        Down,
        Up,
        Page_Up,
        Page_Down,
        Home,
        End,
        Caps_Lock,
        Scroll_Lock,
        Num_Lock,
        Print_Screen,
        Pause,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        KeyPad_0,
        KeyPad_1,
        KeyPad_2,
        KeyPad_3,
        KeyPad_4,
        KeyPad_5,
        KeyPad_6,
        KeyPad_7,
        KeyPad_8,
        KeyPad_9,
        KeyPad_Decimal,
        KeyPad_Divide,
        KeyPad_Multiply,
        KeyPad_Subtract,
        KeyPad_Add,
        KeyPad_Enter,
        KeyPad_Equal,
        Left_Shift,
        Left_Control,
        Left_Alt,
        Left_Super,
        Right_Shift,
        Right_Control,
        Right_Alt,
        Right_Super,
        Menu,
        
        KEYBOARD_END,
        
        //-------------- MOUSE ---------------------
        MOUSE_BEGIN,
        
        Mouse = MOUSE_BEGIN,    ///< always activated (for unpressed position tracking) - this is currently not handled by InputSource but it is assumed that InputNodeSource knows this - we might want to implement it so that we know which device_id it is, but we keep it currently on 0 and I do not see practical problems with it for now
        
        MouseLeft,
        MouseRight,
        MouseMiddle,
        MouseBack,
        MouseForward,
        
        MouseScrollUp,          ///< only triggered
        MouseScrollDown,        ///< only triggered
        MouseScrollLeft,        ///< only triggered
        MouseScrollRight,       ///< only triggered
        
        MOUSE_END,
        
        //-------------- TOUCH ---------------------
        Touch,
    };
    
    
    enum class Type
    {
        Press,
        Release,
        Trigger,
    };
    
    
    Input( Type type, Input::Key key, int device_id, bool real ) :
        _type( type ),
        _key( key ),
        _device_id( device_id ),
        _real( real )
    {
    }
    
    using DeviceKey = std::pair< Input::Key, int >;
    using DeviceKey_Hash = iv::hash< std::pair< Input::Key, int > >;
    
    Type type() const
    {
        return this->_type;
    }
    
    Input::Key key() const
    {
        return this->_key;
    }
    
    int device_id() const
    {
        return this->_device_id;
    }
    
    bool real() const
    {
        return this->_real;
    }
    
private:
    Type            _type;
    Input::Key      _key;
    int             _device_id;
    bool            _real;
};


class InputSource
{
public:
    /**
        Position associated with device specified by key and device_id.
    */
    virtual int2        input_position( Input::Key key, int device_id ) = 0;
    
    /**
        Value associated with given key, typically this are analog triggers and even joysticks on gamepads.
    */
    virtual float       input_value( Input::Key key, int device_id ) = 0;
    
    /**
        When InputSource emmits Input::Key::Character trigger event, this will return the codepoint of the character (until processing finishes).
    */
    virtual unsigned    input_character() = 0;
};


//----------------- StringIO ------------------------------
template<>
struct StringIO< Input::Key > : public StringIO_Table< Input::Key >
{
    static const ValuesType Values;
};

template<>
struct StringIO< Input::Type > : public StringIO_Table< Input::Type >
{
    static const ValuesType Values;
};


}
