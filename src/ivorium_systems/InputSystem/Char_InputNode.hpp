#pragma once

#include "InputNode.hpp"
#include "../TimeSystem/Watch.hpp"

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class Char_InputNode;

class CharListener
{
public:
ClientMarker cm;

                            CharListener( Instance * inst );
                            ~CharListener();
    Instance *              instance() const;
    
    virtual void            on_char( Char_InputNode * event, unsigned codepoint ){};
    
private:
    Instance * inst;
};


class Char_InputNode : public InputNode
{
public:
ClientMarker cm;
using InputNode::instance;

    static const constexpr int RepeatFirstMs = 800;
    static const constexpr int RepeatOtherMs = 40;
    
                            Char_InputNode( Instance * inst, TimeId time_type );
    
    void                    Insert_CharListener( CharListener * );
    void                    Remove_CharListener( CharListener * );
    
protected:
    // InputNode
    virtual bool            input_trigger_process( InputRoot * root, Input::DeviceKey key ) override;
    
private:
    friend class CharListener;
    
    void                    add_char_listener( CharListener * listener );
    void                    remove_char_listener( CharListener * listener );
    
private:
    volatile_set< CharListener * > char_listeners;
};


}
