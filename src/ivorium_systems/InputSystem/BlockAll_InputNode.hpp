#pragma once

#include "InputNode.hpp"

namespace iv
{

class BlockAll_InputNode : public InputNode
{
public:
                        BlockAll_InputNode( Instance * inst ) : InputNode( inst ){}
    virtual bool        input_trigger_process( InputRoot * root, Input::DeviceKey key ){ return false; }
    virtual bool        input_process( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace, bool & queue_refresh ){ return false; }
};

}
