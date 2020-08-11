#pragma once

#include "../Attribute.hpp"
#include "../../Basics/volatile_set.hpp"

namespace iv
{

class Local_AEP : public AttributeEventProcessor
{
public:
                            Local_AEP(){}
                            
                            Local_AEP( Local_AEP const & ) = delete;
    Local_AEP &             operator=( Local_AEP const & ) = delete;

    virtual void            AttributeEvent( AttributeEventProcessor::Event event, Attribute * attr, AttributeListener * listener ) override;
    
private:
    volatile_set< AttributeListener * > listeners;
};

}
