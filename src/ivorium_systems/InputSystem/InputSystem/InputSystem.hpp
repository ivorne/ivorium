#pragma once

#include <ivorium_core/ivorium_core.hpp>

#include <unordered_set>
#include <unordered_map>

namespace iv
{

class InputNode;

/**
*/
class InputSystem : public System
{
public:
                            InputSystem( SystemContainer * sc, InputSource * source );
    virtual                 ~InputSystem(){}
    virtual std::string     debug_name() const override { return "InputSystem"; }
    virtual void            status( TextDebugView * view ) override;
    virtual bool            flushSystem() override;
    
    void                    window_size( int2 );
    int2                    window_size();
    
    int2                    input_position( Input::Key key, int device_id );
    float                   input_value( Input::Key key, int device_id );
    unsigned                input_character();
    
    //--------------------- consistency checking --------------------
    void                    node_register( InputNode * );
    void                    node_unregister( InputNode * );
    
private:
    InputSource * source;
    int2 _window_size;
    
    std::unordered_set< InputNode * > nodes;
    unsigned last_check_frame_id;
};

}
