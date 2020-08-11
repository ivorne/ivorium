#include "Heap.hpp"

namespace iv
{

Heap::Heap( Instance * inst, ClientMarker * cm ) :
    inst( inst ),
    cm( cm ),
    items()
{
}

Heap::~Heap()
{
    for( Item const & item : iv::reversed( this->items ) )
        item.deleter( item.identity );
}

void Heap::clear()
{
    for( Item const & item : iv::reversed( this->items ) )
        item.deleter( item.identity );
    this->items.clear();
}

}
