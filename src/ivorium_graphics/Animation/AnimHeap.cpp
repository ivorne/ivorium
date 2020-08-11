#include "AnimHeap.hpp"
#include "Animation/AnimSystem.hpp"

namespace iv
{

AnimHeap::AnimHeap( Instance * inst ) :
    cm( inst, this, "AnimHeap" ),
    inst( inst ),
    heap( inst, &this->cm )
{
}

AnimHeap::~AnimHeap()
{
}

Instance * AnimHeap::instance() const
{
    return this->inst;
}

Lambda_Connector * AnimHeap::Make_Lambda_Connector()
{
    return this->heap.template createClient< iv::Lambda_Connector >();
}

AnimNodeI * AnimHeap::VoidNode()
{
    auto node = this->heap.template createClient< iv::AnimNodeI >();
    this->nodes.insert( node );
    return node;
}

}
