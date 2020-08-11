#include "AnimConnector.hpp"
#include "AnimNode.hpp"

namespace iv
{

AnimConnector::AnimConnector( Instance * inst ) :
    cm( inst, this, "AnimConnector" ),
    inst( inst ),
    _parent_changed( false ),
    _child_arrived( false )
{
}

Instance * AnimConnector::instance() const
{
    return this->inst;
}

void AnimConnector::Activate()
{
    this->anim_eachParent(
        [&]( AnimNodeI * parent )
        {
            parent->Activate();
        }
    );
}

void AnimConnector::parentChanged_Set()
{
    this->_parent_changed = true;
}

bool AnimConnector::parentChanged_Get()
{
    return this->_parent_changed;
}

void AnimConnector::parentChanged_Clear()
{
    this->_parent_changed = false;
}

void AnimConnector::childArrived_Set()
{
    this->_child_arrived = true;
}

bool AnimConnector::childArrived_Get()
{
    return this->_child_arrived;
}

void AnimConnector::childArrived_Clear()
{
    this->_child_arrived = false;
}

}
