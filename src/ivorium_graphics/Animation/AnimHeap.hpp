#pragma once

#include "Connectors/Destination_AnimNode.hpp"
#include "Connectors/Source_AnimNode.hpp"
#include "Connectors/State_AnimNode.hpp"
#include "Connectors/Lambda_Connector.hpp"
#include "Connectors/Cooldown_Connector.hpp"
#include "Connectors/TimeTransform_Connector.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <unordered_set>

namespace iv
{

class AnimHeap
{
public:
ClientMarker cm;
                                    AnimHeap( Instance * inst );
                                    ~AnimHeap();
                            
    Instance *                      instance() const;
    
    std::unordered_set< AnimNodeI * > const & Debug_Nodes() const { return this->nodes; }
    
    //--------------------- Nodes ------------------------------------
    template< class T >
    AnimNode< T > *                 Node( T const & initial_value );
    
    /**
        Node without value.
        Usualy used as neutral root node that advances N-N type connectors, like for example Lambda_Connector.
    */
    AnimNodeI *                     VoidNode();
    
    template< class T >
    AnimNode< T > *                 Attribute_SourceNode( Attr< T > * attr, T const & default_value );
    
    template< class T >
    AnimNode< T > *                 Attribute_DestinationNode( Attr< T > * attr, T const & initial_value );
    
    /**
        This has no real output effect, but it traverses towards target value with given speed.
        It can be used for timing and intermediate state storage.
    */
    template< class T >
    AnimNode< T > *                 State_DestinationNode( T const & initial_value );
    
    //--------------------- Connectors -------------------------------
    Lambda_Connector *              Make_Lambda_Connector();
    
    template< class T >
    Cooldown_Connector< T > *       Make_Cooldown_Connector( AnimNode< T > * parent, AnimNode< T > * child );
    
    template< class T >
    TimeTransform_Connector< T > *  Make_TimeTransform_Connector( TimeTransform const &, AnimNode< T > * parent, AnimNode< T > * child );
    
private:
    Instance * inst;
    Heap heap;
    
    std::unordered_set< AnimNodeI * > nodes;
};


}

#include "AnimHeap.inl"
