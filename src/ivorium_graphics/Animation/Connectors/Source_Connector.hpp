#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
*/
template< class T >
class Source_Connector : public Field< T >
{
public:
ClientMarker cm;

                            Source_Connector( Instance * inst, T const & default_value, AnimNode< T > * child = nullptr, Attr< T > * source = nullptr );
    Instance *              instance() const;
    
    void                    SetSource( Attr< T > * source )     { this->Assign_Attribute_R( source ); }
    void                    SetChild( AnimNode< T > * child );
    
private:
    //--------------------- Field ---------------------------------------
    virtual void            OnChanged( bool real ) override;
    
private:
    Instance * inst;
    AnimNode< T > * child;
    T default_value;
};


}

#include "Source_Connector.inl"
