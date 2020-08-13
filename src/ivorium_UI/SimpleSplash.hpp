#pragma once

#include <ivorium_graphics/ivorium_graphics.hpp>

namespace iv
{

/**
    \ingroup Elements
    \brief Splash screen that is visible during loading times.
    
    Can be put above of everything else in scene.
    It will show up when something is being loaded (like textures) and hide when loading is done. 
    We should modify this to allow to select custom texture.
*/
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
