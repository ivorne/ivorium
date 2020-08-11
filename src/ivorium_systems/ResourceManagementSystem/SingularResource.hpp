#pragma once

#include "ResourceAccess.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
*/
template< class RT >
class SingularResource
{
public:
ClientMarker cm;

                                                SingularResource( Instance * inst, ResourcePath const & path = ResourcePath() );
    Instance *                                  instance() const;
    
    void                                        path( ResourcePath const & path );

    RT const *                                  get() const;
    RT const &                                  operator*() const;
    RT const *                                  operator->() const;
    
private:
    ResourceAccess access;
    RT const * resource;
};

}

#include "SingularResource.inl"
