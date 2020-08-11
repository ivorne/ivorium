#pragma once

#include "../TimeSystem/Watch.hpp"
#include "../DVarSystem/DVar.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    It is required to call InputEvent::enable in constructor and InputEvent::disable in destructor of each child that overrides any InputEvent method.
*/
class InputEvent : private PrivField_Owner< Activator >, private PrivField_Owner< SumAgg< int > >, protected PrivValAttr_Owner
{
public:
//----------------------------- instance control -------------------
ClientMarker cm;
    
                                    InputEvent( Instance * inst );
                                    ~InputEvent();
    
    Instance *                      instance() const;
    void                            status( iv::TableDebugView * view );
    
//--------------------------------- observers ------------------
    /**
    */
    PrivValAttr< bool >             attr_eventEnabled;
    
    /**
        Used for visualization and state tracking.
        This does not necessarily reflect just the input state but rather how that input is projected into destination attributes (through Field_RW's).
    */
    PrivValAttr< Activator >        attr_activation;
    
    /**
        This is enabled when either duration is enabled or when some activation is hot (key is pressed and activation can happen as result of it in the future).
    */
    PrivValAttr< bool >             attr_hot;
    
    /**
        Logic that should be controlled by this input event will be connected to one or more of these fields.
    */
    PrivField< Activator >          field_press;
    PrivField< Activator >          field_release;
    PrivField< SumAgg< int > >      field_duration;
    
//--------------------------------- called by implementors -------------------------------------------
protected:
    void                            ie_start_duration();
    void                            ie_stop_duration();
    
    void                            ie_trigger();
    
    void                            ie_start_activation();
    void                            ie_stop_activation( bool real );
    
private:
    virtual void                    Field_OnChanged( Field< Activator > * field, bool real ) override;
    virtual void                    Field_OnChanged( Field< SumAgg< int > > * field, bool real ) override;
    
    void                            refresh_enabled();
    
    void                            check_consistency();
    
private:
    Instance * inst;
    
    bool                            release_hot;
    bool                            duration;
    
    Activator                       press_reference;
    Activator                       release_reference;
};

}
