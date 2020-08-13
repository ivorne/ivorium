#pragma once

#include "LumaScroller.hpp"
#include <ivorium_graphics/ivorium_graphics.hpp>

namespace iv
{

/**
    \ingroup Elements
    \brief A window-like scrollable frame that needs to be supplied a texture.
    
    Has vertically scrolling content implemented using \ref LumaScroller.
    Just set texture path to \ref LumaFrame::frame and set \ref LumaFrame::frameWidth and add children to \ref LumaFrame::child and everything should work.
*/
class LumaFrame : public iv::ProtectedSlot, protected iv::PrivValAttr_Owner
{
public:
iv::ClientMarker cm;
using iv::ProtectedSlot::instance;

                                                LumaFrame( iv::Instance * );
    
    LumaFrame *                                 frame( iv::ResourcePath );
    LumaFrame *                                 frameWidth( float );
    LumaFrame *                                 open( bool );

    iv::DirtyAttr< iv::ResourcePath >           attr_frame;
    iv::DirtyAttr< float >                      attr_frameWidth;
    iv::DirtyAttr< bool >                       attr_open;      ///< If frame should be visible (open). There is a short transition between open and closed state.
    
    iv::PrivValAttr< iv::SlotChild * >          child;
    
protected:
    // ProtectedSlot
    virtual void                                first_pass_impl( iv::ElementRenderer * ) override;
    
    // PrivValAttr_Owner
    virtual void                                PrivValAttr_Modified( Attribute * ) override;
    
private:
    iv::AnimHeap anim;
    iv::LumaScroller * scroller;
    iv::Image * img_frame;
    iv::Lambda_Connector * connector;
    iv::Border * border;
};

}
