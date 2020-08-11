#pragma once

#include <ivorium_core/ivorium_core.hpp>

#include <set>
#include <utility>

namespace iv
{

class InputNode;
class InputSystem;

class InputRoot
{
public:
    virtual void        treeRefresh() = 0;      ///< Typically called when structure of input processing tree changes. It sends all active inputs through the tree again, so that it can adapt to the new state of input sorting.
    virtual void        deactivateSubtree( InputNode * node ) = 0;
    virtual void        reserve_key( Input::DeviceKey key, bool reserve ) = 0;
};

/**
    Order of traversal - lowest order child of root, then children of the child (recursively) then higher order children of root with their children.
*/
class InputNode
{
public:
friend class InputSystem;   // Only used for debugging prints.
ClientMarker cm;
                            InputNode( Instance * inst );
                            ~InputNode();
    
    Instance *              instance() const;
    void                    status( iv::TableDebugView * view );
    
    /**
        Disables node and its whole subtree.
    */
    void                    inputEnabled( bool );
    bool                    inputEnabled();
    
    /**
        This node being disconnected from scene tree will not be reported in ConsistencyChecks log.
    */
    void                    quiet( bool );
    bool                    quiet() const;
    
    /**
        Called when InputNode is removed from the tree.
    */
    void                    input_deactivate();
    
    /*
        Call this when:
            1. Method input_eachChild would give different set of nodes or would give them in different order (if that is relevant for input priority).
            2. Method input_trigger_process or input_process trigger would accept different inputs than before.
        This is the same as calling InputRoot::treeChanged.
        This should be called on InputNode that is already part of the tree, because it uses pointer to last InputRoot to inform the root about this change.
    */
    void                    input_treeRefresh();
    
    //--------------------- input tree ----------------------------------
    /**
        Parents InputNode::childDisconnect is called either from destructor and when the node is about to be registered as child to another parent.
        Important: Do not call this directly from general outside code. It is called from containers to setup callbacks about when the child gets disconnected..
    */
    void                    input_setParent( InputNode * );
    
    /**
    */
    InputNode *             input_getParent();
    
    /**
    */
    virtual InputRoot *     input_getRoot();
    
    /**
        Should consistently give the same nodes and in the same order, if they could react to the same inputs, causing need for priority resolution.
        When the given children change, InputNode::input_deactivate should be called on all children that were removed from the set and input_treeChanged should be called
    */
    virtual void            input_eachChild( std::function< void( InputNode * ) > const & ){}
    
    /**
    */
    virtual void            input_childDisconnect( InputNode * ){}
    
    //--------------------- end point input processing --------------------------------------------------------
    /**
        Returns true if we should continue to following nodes.
    */
    bool                    input_trigger_visit( InputRoot * root, Input::DeviceKey key );

    /**
        This sends input to self and all children in correct order until some InputNode processes it.
        Returns true if we should continue to following nodes.
    */
    bool                    input_visit( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace );
    
protected:
    //--------------------- end-point input processing ------------------------
    /**
        Returns true if we should continue to following nodes.
    */
    virtual bool            input_trigger_process( InputRoot * root, Input::DeviceKey key ){ return true; }
    
    /**
        This should behave just like a set - group of keys is accepted, all others are not accepted.
        When this changes, request_refresh() on root should be called to apply the change.
        Returns true if we should continue to following nodes.
    */
    virtual void            input_process( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace ){}
    
private:
    Instance * inst;
    bool _enabled;
    InputNode * _parent;
    bool _quiet;
};

}
