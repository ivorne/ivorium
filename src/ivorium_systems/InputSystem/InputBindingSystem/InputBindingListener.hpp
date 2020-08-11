#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class InputBindingSystem;

class InputBindingListener
{
public:
                        InputBindingListener( Instance * inst );
                        ~InputBindingListener();
    
    Instance *          instance() const;
    
    /**
        TODO - Think about how to aggregate multiple consequent calls to this to a single one.
    */
    virtual void        on_binding_changed() = 0;
    
private:
    Instance * inst;
    InputBindingSystem * ibs;
};

}
