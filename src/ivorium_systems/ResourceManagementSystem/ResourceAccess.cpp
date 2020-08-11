#include "ResourceAccess.hpp"

namespace iv
{

ResourceAccess::ResourceAccess( Instance * inst ) :
    cm( inst, this, "ResourceAccess" ),
    inst( inst ),
    rms( inst->getSystem< ResourceManagementSystem >() )
{
}

Instance * ResourceAccess::instance() const
{
    return this->inst;
}

}
