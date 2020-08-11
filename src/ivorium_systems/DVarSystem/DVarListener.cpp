#include "DVarListener.hpp"
#include "DVarSystem.hpp"

namespace iv
{

DVarListener::DVarListener( Instance * inst ) :
    inst( inst ),
    dvs( inst->getSystemContainer()->getSystem< DVarSystem >() )
{
}

DVarListener::~DVarListener()
{
    if( this->dvs )
        for( DVarId id : this->listenings )
            this->dvs->remove_listener( this, id );
}

Instance * DVarListener::instance()
{
    return this->inst;
}

void DVarListener::start_dvar_listening( DVarId id )
{
    if( this->dvs )
    {
        this->listenings.insert( id );
        this->dvs->add_listener( this, id );
    }
}

void DVarListener::stop_dvar_listening( DVarId id )
{
    if( this->dvs )
    {
        this->listenings.erase( id );
        this->dvs->remove_listener( this, id );
    }
}

}
