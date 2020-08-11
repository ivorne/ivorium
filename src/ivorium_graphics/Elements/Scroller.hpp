#pragma once

#include "Elem.hpp"
#include "Slot.hpp"
#include "OneChildElem.hpp"
#include <ivorium_systems/ivorium_systems.hpp>
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
*/
class Scroller : public OneChildElem< SlotChild >, public SlotChild, private FixedUpdateClient, private FrameUpdateClient, protected PrivValAttr_Owner
{
public:
ClientMarker cm;
using Elem::instance;

                                Scroller( Instance * );
    void                        status( iv::TableDebugView * view );
    
    Scroller *                  input_id( InputId );
    InputId                     input_id();
    
// for visualization
    PrivValAttr< float >        attr_position;
    PrivValAttr< float >        attr_outsize;
    PrivValAttr< float >        attr_insize;
    PrivValAttr< float >        attr_overlapTop;
    PrivValAttr< float >        attr_overlapBottom;
    
// utility methods
    
    // Elem
    Scroller *                  enabled( bool );
    
protected:
    // Elem
    virtual void                first_pass_impl( ElementRenderer * ) override;
    virtual void                second_pass_impl( ElementRenderer * ) override;
    
    // InputNode
    virtual void                input_process( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace ) override;
    virtual bool                input_trigger_process( InputRoot * root, Input::DeviceKey key ) override;
    
    // FixedUpdateClient
    virtual void                fixed_update( TimeId time, int time_step, int steps ) override;
    
    // FrameUpdateClient
    virtual void                frame_update() override;
    
private:
    void                        clamp_position();
    void                        accept_candidate();
    float2                      input_position( Input::DeviceKey key );
    void                        reserve_hover_keys( InputRoot * root, bool reserve );
    void                        refresh_clamp_countdown();
    
private:
    //
    DirtyAttr< float >              position;
    DirtyAttr< float >              smoothedPosition;
    
    // drag and drop input
    InputBindingQuery ibq;
    InputQuery iq;
    
    std::optional< Input::DeviceKey >   _candidate;
    std::optional< Input::DeviceKey >   _active;
    float                               _candidate_pos;
    float                               _residual_speed;
    int                                 _full_clamp_countdown;
    
    InputId _input_id;
};

}
