#include "Char_InputNode.hpp"

#include "InputSystem/InputQuery.hpp"
#include "../Defs.hpp"

namespace iv
{

//------------------------- CharListener -----------------------------
CharListener::CharListener( Instance * inst ) :
    cm( inst, this, "CharListener" ),
    inst( inst )
{
}

CharListener::~CharListener()
{
}

Instance * CharListener::instance() const
{
    return this->inst;
}

//--------------------------- Char_InputNode --------------------------------------------
Char_InputNode::Char_InputNode( Instance * inst, TimeId time_type ) :
    cm( inst, this, "Char_InputEvent" ),
    InputNode( inst )
{
    this->cm.inherits( this->InputNode::cm );
}

void Char_InputNode::Insert_CharListener( CharListener * listener )
{
    this->char_listeners.insert( listener );
}

void Char_InputNode::Remove_CharListener( CharListener * listener )
{
    this->char_listeners.erase( listener );
}

bool Char_InputNode::input_trigger_process( InputRoot * root, Input::DeviceKey key )
{
    if( key.first != Input::Key::Character )
    {
        this->cm.log( SRC_INFO, Defs::Log::Input, "Refuse input, not a Character input." );
        return true;
    }
    
    this->cm.log( SRC_INFO, Defs::Log::Input, "Accept Character input." );
    
    InputQuery iq( this->instance() );
    unsigned character = iq.input_character();
    
    this->char_listeners.foreach(
        [&]( CharListener * listener )
        {
            listener->on_char( this, character );
        }
    );
    
    return false;
}

}
