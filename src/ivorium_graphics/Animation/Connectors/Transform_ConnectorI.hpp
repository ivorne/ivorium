#pragma once

#include "../../Defs.hpp"
#include "../Animation/AnimNode.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    Abstract class for all connectors that have one parent node and one child node and it somehow copies target value from parent node to child node.
    These connectors will usually do some kind of value transformation or time / delay transformation.
*/
template< class TParent, class TChild >
class Transform_ConnectorI : protected AnimConnector
{
public:
using AnimConnector::instance;
ClientMarker cm;

//----------------------------- Transform_ConnectorI -------------------------------------------------------------------
                                Transform_ConnectorI( Instance * inst );
                                ~Transform_ConnectorI();
    
    //------------------------- AnimConnector ------------------------------------------------------
    virtual void                anim_eachChild( std::function< void( AnimNodeI * ) > const & ) override;
    virtual void                anim_childDisconnect( AnimNodeI * node ) override;
    
    virtual void                anim_eachParent( std::function< void( AnimNodeI * ) > const & ) override;
    virtual void                anim_parentDisconnect( AnimNodeI * node ) override;
    
    //------------------------- structure ------------------------------------------------------
    /**
    */
    void                        SetParent( AnimNode< TParent > * node );
    
    /**
    */
    void                        SetChild( AnimNode< TChild > * node );
    
protected:
    bool                        treeChanged_Get();
    void                        treeChanged_Clear();
    
protected:
    AnimNode< TParent > *       parent;
    AnimNode< TChild > *        child;
    
private:
    bool _treeChanged;
};

}

#include "Transform_ConnectorI.inl"
