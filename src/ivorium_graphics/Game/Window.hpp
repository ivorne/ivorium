#pragma once

#include "WindowListener.hpp"
#include "../OpenGL/RenderTarget.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class Window : public InputSource
{
public:
    virtual void                    set_listener( WindowListener * listener ) = 0;
    virtual bool                    gpu_enabled() = 0;
    virtual RenderTarget::Geometry  geometry() = 0;
    virtual RenderTarget *          render_target() = 0;
    
protected:
    void                            setupGL();
    
private:
};

}
