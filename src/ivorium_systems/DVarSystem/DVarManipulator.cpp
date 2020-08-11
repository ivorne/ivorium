#include "DVarManipulator.hpp"
#include "DVarSystem.hpp"

namespace iv
{

DVarManipulator::DVarManipulator( Instance * inst ) :
    cm( inst, this, "DVarManipulator" ),
    inst( inst ),
    dvs( inst->getSystem< DVarSystem >() )
{
}

Instance * DVarManipulator::instance()
{
    return this->inst;
}

}
