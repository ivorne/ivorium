#pragma once

#include "AndroidAssetResourceProvider.hpp"
#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{


class PlatformDefault_ResourceProvider
{
public:
ClientMarker cm;

                    PlatformDefault_ResourceProvider( Instance * inst, size_t priority );
    Instance *      instance() const; 
    
private:
    #if IV_FSPLATFORM_XDG || IV_FSPLATFORM_WIN || IV_FSPLATFORM_EMSCRIPTEN
    FSResourceProvider              provider;
    #elif IV_FSPLATFORM_ANDROID
    AndroidAssetResourceProvider    provider;
    #else
    #error "Unimplemented FS Platform."
    #endif
};

}
