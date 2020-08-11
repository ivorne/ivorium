#pragma once

#include "InputNode.hpp"
#include "InputBindingSystem/InputBindingListener.hpp"
#include "../TimeSystem/FrameUpdateClient.hpp"

namespace iv
{

class SourceInputNode : public InputNode, public InputRoot, private InputBindingListener, public FrameUpdateClient
{
public:
using InputNode::instance;
ClientMarker cm;

                            SourceInputNode( Instance * inst );
                            ~SourceInputNode();
    
    bool                    input( Input const * input );
    
    void                    Add_InputNode( InputNode * node );
    void                    Remove_InputNode( InputNode * node );
    
    // InputRoot
    virtual void            treeRefresh() override;
    virtual void            deactivateSubtree( InputNode * node ) override;
    virtual void            reserve_key( Input::DeviceKey key, bool reserve ) override;
    
    // InputNode
    virtual InputRoot *     input_getRoot() override;
    virtual void            input_eachChild( std::function< void( InputNode * ) > const & ) override;
    virtual void            input_childDisconnect( InputNode * ) override;
    
private:
    // FrameUpdateClient
    virtual void            frame_update() override;
    
    // InputBindingListener
    virtual void            on_binding_changed() override;
    
    //
    void                    do_refresh();
    bool                    is_reserved( Input::DeviceKey device_key );
    
private:
    std::unordered_set< Input::DeviceKey, Input::DeviceKey_Hash > pressed;
    std::unordered_set< Input::DeviceKey, Input::DeviceKey_Hash > reserved;
    std::unordered_set< Input::Key > reserved_whole;
    
    bool refresh_queued;
    
    std::vector< InputNode * > _input_children;
};

}
