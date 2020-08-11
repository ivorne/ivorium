#pragma once

#include "Elem.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
*/
template< class ChildT >
class OneChildElem : public Elem
{
public:
ClientMarker cm;
    
                            OneChildElem( Instance * inst );
                            ~OneChildElem();
    
    /**
        Child must specialize ChildT class.
    */
    template< class T, class ... CArgs >
    T *                     createChild( CArgs const & ... cargs );
    
    template< class T, class ... CArgs >
    T *                     createChild_Instance( char const * name, CArgs const & ... cargs );
    
    void                    setChild( ChildT * );
    void                    removeChild();
    
protected:
    // initialziation parameter (managed by methods)
    DirtyAttr< ChildT * > child;
    
    // Elem
    virtual void            elem_eachChild( std::function< void( Elem * ) > const & ) override;
    virtual void            elem_childDisconnect( Elem * ) override;
    
    // InputNode
    virtual void            input_eachChild( std::function< void( InputNode * ) > const & f ) override;
    virtual void            input_childDisconnect( InputNode * child ) override;
    
private:
    Heap heap;
};

}

#include "OneChildElem.inl"
