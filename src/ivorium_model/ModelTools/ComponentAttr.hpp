#pragma once

#include <ivorium_core/ivorium_core.hpp>
#include <unordered_map>

namespace iv
{

//=================================== ComponentAttr_Index ===========================================
/**
    \ingroup ModelTools
*/
template< class Component >
class ComponentAttr_Index
{
public:
iv::ClientMarker cm;
                            ComponentAttr_Index( iv::Instance * inst );
    iv::Instance *          instance() const;
    
    void                    AttributeEvent( iv::AttributeEventProcessor::Event event, Component * component, iv::Attribute * attr, iv::AttributeListener * listener );
    
protected:
    virtual void            Component_AttrChanged( Component * component, iv::Attribute * attr ) = 0;
    
private:
    iv::Instance * inst;
    std::unordered_multimap< iv::Attribute *, iv::AttributeListener * > listeners;
    bool iterating;
};

//====================================== ComponentAttr ==============================================
/**
    \ingroup ModelTools
*/
template< class Component, class T >
class ComponentAttr final : private iv::AttributeEventProcessor, public iv::SharedAttr_I< T >
{
public:
                            ComponentAttr( iv::ClientMarker * cm, T const & initial_value = T() );
                            ~ComponentAttr();
    
                            ComponentAttr( ComponentAttr< Component, T > const & src );
    ComponentAttr< Component, T > & operator=( ComponentAttr< Component, T > const & src );
    
    /**
        These references are not copied in copy constructor and assignment operator.
    */
    void                    Index( Component * component, ComponentAttr_Index< Component > * index );
    
    /**
        Same as ComponentAttr< Component, T >::Index( nullptr, nullptr ).
    */
    void                    Index();
    
private:
    virtual void            AttributeEvent( iv::AttributeEventProcessor::Event event, iv::Attribute * attr, iv::AttributeListener * listener ) override;
    
private:
    Component * component;
    ComponentAttr_Index< Component > * index;
};

}

#include "ComponentAttr.inl"
