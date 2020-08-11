#include "Local_AEP.hpp"

namespace iv
{

void Local_AEP::AttributeEvent( AttributeEventProcessor::Event event, Attribute * attr, AttributeListener * new_listener )
{
    switch( event )
    {
        case AttributeEventProcessor::Event::AddListener:
        {
            this->listeners.insert( new_listener );
        } break;
        
        case AttributeEventProcessor::Event::RemoveListener:
        {
            this->listeners.erase( new_listener );
        } break;
        
        case AttributeEventProcessor::Event::Activated:
        {
            this->listeners.foreach(
                [&]( AttributeListener * listener )
                {
                    listener->Attribute_Activated( attr );
                }
            );
        } break;
        
        case AttributeEventProcessor::Event::Deactivated:
        {
            this->listeners.foreach(
                [&]( AttributeListener * listener )
                {
                    listener->Attribute_Deactivated( attr );
                }
            );
        } break;
        
        case AttributeEventProcessor::Event::Changed:
        {
            this->listeners.foreach(
                [&]( AttributeListener * listener )
                {
                    listener->Attribute_Changed( attr );
                }
            );
        } break;
    }
}

}
