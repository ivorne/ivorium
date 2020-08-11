#pragma once

#include "DVarId.hpp"
#include "DVarSystem.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{
    
/**
 * NOTE - context now does not duplicate DVarSystem; this must be done manualy, or maybe I need to think about it and do it better
 *      - automatic DVarSystem caused confusion often (user does not know how many DVarSystems there are; also DVarSystem was not created for current SystemContainer, so problems
 * 
    This instance separates DVar contexts.
    Children of this instance will use different DVarSystem than parents.
    Created DVarSystem has pointer to original (parent) DVarSystem so if a value is not explicitly specified in this context, value from parent context (parent DVarSystem) will be used.
    New context is used by children, DVarManipulator itself uses old (parent) context.
    
    Multiple DVarManipulator instances can be active on one Instance, they should not interfere with each other in any harmful way.
*/
class DVarManipulator
{
public:
ClientMarker cm;

                    DVarManipulator( Instance * inst );
    Instance *      instance();
    
    template< class Type >
    void             set( DVarIdT< Type > id, Type const & value );
    
private:
    Instance * inst;
    DVarSystem * dvs;
};

}

#include "DVarManipulator.inl"
