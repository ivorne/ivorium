#pragma once

#include <ivorium_graphics/ivorium_graphics.hpp>

namespace iv
{

class SimpleSplash : public Slot, public DelayedLoader
{
public:
ClientMarker cm;
    SimpleSplash( Instance * inst );
    
protected:
    virtual void LoadStart() override;
    virtual void LoadFinish() override;
    
private:
    AnimHeap anim;
    AnimNode< bool > * in_enabled;
    Lambda_Connector * lambda;
};

}
