#pragma once

#include "Slot.hpp"
#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

template< class ChildT >
class VectorChildrenElem : public Elem
{
public:
ClientMarker cm;
    
                                    VectorChildrenElem( Instance * inst );
                                    ~VectorChildrenElem();
    
    /**
        Child must specialize ChildT class.
    */
    template< class T, class ... CArgs >
    T *                             createChild( CArgs const & ... cargs );
    
    template< class T, class ... CArgs >
    T *                             createChild_Instance( char const * name, CArgs const & ... cargs );
    
    void                            pushChild( ChildT * );
    void                            removeChild( ChildT * );
    
protected:
// initialization parameters
    std::vector< ChildT * >         children;
    bool                            children_dirty;
    
    // Elem
    virtual void                    elem_eachChild( std::function< void( Elem * ) > const & ) override;
    virtual void                    elem_childDisconnect( Elem * ) override;
    
    // InputNode
    virtual void                    input_eachChild( std::function< void( InputNode * ) > const & f ) override;
    virtual void                    input_childDisconnect( InputNode * child ) override;
    
private:
    void                            _removeChild( size_t i );
    Heap heap;
};

}

#include "VectorChildrenElem.inl"
