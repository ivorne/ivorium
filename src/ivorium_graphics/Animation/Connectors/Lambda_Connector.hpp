#pragma once

#include "../Animation/AnimNode.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <type_traits>

namespace iv
{

/**
*/
class Lambda_Connector : private AnimConnector
{
public:
using AnimConnector::instance;
ClientMarker cm;

//----------------------------- Lambda_Connector -------------------------------------------------------------------
                                Lambda_Connector( Instance * inst );
                                ~Lambda_Connector();
    
    //------------------------- AnimConnector ------------------------------------------------------
    virtual void                anim_eachChild( std::function< void( AnimNodeI * ) > const & ) override;
    virtual void                anim_childDisconnect( AnimNodeI * node ) override;
    
    virtual void                anim_eachParent( std::function< void( AnimNodeI * ) > const & ) override;
    virtual void                anim_parentDisconnect( AnimNodeI * node ) override;
    
    virtual void                UpdatePass_Down() override;
    virtual void                UpdatePass_Up() override;
    
    //------------------------- parents nodes ------------------------------------------------------
    /**
    */
    Lambda_Connector *          AddParent( AnimNodeI * node );
    Lambda_Connector *          AddChild( AnimNodeI * node );
    
    /**
        This lambda should set correct targets to (presumably caputured) output nodes.
        If warping is set to true, retargeting lambda should target an equilibrium state of the animation.
    */
    void                        Retargeting( std::function< void( bool warping ) > const & );
    
    /**
    */
    void                        Retarget( bool warping = false );
    
private:
    std::unordered_set< AnimNodeI * > _parents;
    std::unordered_set< AnimNodeI * > _children;
    std::function< void( bool ) > _retargeting;
    bool _retarget;
};

}
