#pragma once

#include "ResourceManagementSystem.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    When a resource is created, it is bound (Instance::bind) to one proxy Instance owned by ResourceManagementSystem (so the resource is effectively bound to the system).
    Users of ResourceAccess should not bind returned Instances to anything (might be changed in future).
    Currently, resources are never deallocated, but it is encouraged to use weak pointers to resources (will be useful in future).
    Some deallocation mechanisms should be done later, when the need arises.
*/
class ResourceAccess
{
public:
ClientMarker cm;

                        ResourceAccess( Instance * inst );
    Instance *          instance() const;

    template< class TResource >
    bool                has_path( ResourcePath const & path );

    template< class TResource >
    TResource const *   get( ResourcePath const & path );
    
    /**
        Single ResourcePath can get returned multiple times - if more than one resource provider provides it - but only one instance with given ResourcePath is accessible (from provider with highest priority).
    */
    template< class TResource >
    void                each_resource( std::function< void( ResourcePath const & path ) > const & );

private:
    Instance * inst;
    ResourceManagementSystem * rms;
};

}

#include "ResourceAccess.inl"
