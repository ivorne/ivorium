#pragma once

#include "../Animation/AnimNode.hpp"

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class Activator_AnimNode : public iv::AnimNode< iv::Activator >
{
public:
    Activator_AnimNode( iv::Instance * inst ) :
        iv::AnimNode< iv::Activator >( inst, iv::Activator() )
    {
    }

    void Activate()
    {
        this->Target( this->Target().MakeActivated() );
    }
};

}
