#pragma once

#include "TimeTransform.hpp"
#include "Transform_ConnectorI.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <cmath>

namespace iv
{

/**
*/
template< class T >
class TimeTransform_Connector : public Transform_ConnectorI< T, T >
{
public:
using Transform_ConnectorI< T, T >::instance;
ClientMarker cm;

//----------------------------- TimeTransform_Connector -------------------------------------------------------------------
                                TimeTransform_Connector( Instance * inst );
    void                        status( iv::TableDebugView * view );
    
    //-------------------------- configuration -----------------------------------------------------
    /**
        The lambda should not contain any pointers or references (this AnimConnector is not meant to be used for callbacks, use something else for it).
        This is by default linear transformation (no transformation).
    */
    void                        transform( TimeTransform const & );
    
protected:
//----------------------------- AnimConnector ------------------------------------------------------
    virtual void                UpdatePass_Down() override;
    virtual void                UpdatePass_Up() override;
    
private:
    TimeTransform               _transform;
    
    Anim_float                  current_source_time;
    Anim_float                  current_total_time;
    
    Anim_float                  current_distance;
    std::optional< Anim_float > reset_remainder_distance;

    static const constexpr Anim_float DistanceChangeRatioThatCausesReset = 0.05;
};

extern iv::TableId TimeTransform_Connector_DebugTable;

}

#include "TimeTransform_Connector.inl"
