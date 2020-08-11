#pragma once

#include "Camera.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <string>
#include <unordered_set>

namespace iv
{

/**
    This is here just to track Camera instances for debug printing and Elem instances for tree consistency checking.
*/
class ElementSystem : public System
{
public:
                            ElementSystem( SystemContainer * sc );
    virtual std::string     debug_name() const override { return "ElementSystem"; }
    virtual void            status( TextDebugView * view ) override;
    virtual bool            flushSystem() override;
    
    // Camera
    void                    camera_created( Camera * );
    void                    camera_destroyed( Camera * );
    
    // Elem
    void                    elem_register( Elem * );
    void                    elem_unregister( Elem * );
    
private:
    void                    print_elem_children( TreeDebugView * view, Elem * elem, std::unordered_set< Elem * > & printed );
    
private:
    std::unordered_set< Camera * > cameras;
    std::unordered_set< Elem * > elems;
    unsigned last_check_frame_id;
};

}
