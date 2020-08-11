#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

using Anim_float = Interpolator_float;

class AnimNodeI;

template< class T >
class AnimNode;

/**
*/
class AnimConnector
{
public:
ClientMarker cm;

                            AnimConnector( Instance * inst );
    Instance *              instance() const;
    
    void                    Activate();
    
    //--------------------- anim graph ----------------------------------------------------
    virtual void            anim_eachChild( std::function< void( AnimNodeI * ) > const & ){}
    virtual void            anim_childDisconnect( AnimNodeI * node ){}
    
    virtual void            anim_eachParent( std::function< void( AnimNodeI * ) > const & ){}
    virtual void            anim_parentDisconnect( AnimNodeI * node ){}
    
    //-------------------------------------------------------------------------------------
    /**
        Called by parent nodes when their target changes.
    */
    void                    parentChanged_Set();
    bool                    parentChanged_Get();
    void                    parentChanged_Clear();
    
    /**
        Called by children nodes when they reach destination (AnimNodeI::IsInTarget changes to true).
    */
    void                    childArrived_Set();
    bool                    childArrived_Get();
    void                    childArrived_Clear();
    
    //-------------------------------------------------------------------------------------
    /**
        Called during update anim tree traversal on the way down.
        All parent nodes have been updated.
        Sets targets and requests updates on child nodes.
        Can use parent targets and parent distance requests.
    */
    virtual void            UpdatePass_Down() = 0;
    
    /**
        Called during update anim tree traversal on the way up.
        Children nodes were updated using targets and distances configured in RequestUpdates.
        This uses new distances of child nodes to compute new distances of parent nodes.
    */
    virtual void            UpdatePass_Up() = 0;
    
private:
    Instance * inst;
    bool _parent_changed;
    bool _child_arrived;
};

}
