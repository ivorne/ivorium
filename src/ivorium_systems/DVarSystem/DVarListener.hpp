#pragma once

#include "DVarId.hpp"
#include "DVarSystem.hpp"
#include <unordered_set>

namespace iv
{

class Instance;
class DVarSystem;

/**
    Listens to changes on selected DVars.
*/
class DVarListener : public DVarListenerI
{
public:
                    DVarListener( Instance * inst );
                    ~DVarListener();
    Instance *      instance();

    void            start_dvar_listening( DVarId id );
    void            stop_dvar_listening( DVarId id );
    
    virtual void    on_dvar_changed( DVarId id, std::type_index type, void * value ) override = 0;
    
private:
    Instance * inst;
    DVarSystem * dvs;
    std::unordered_set< DVarId > listenings;
};

}
