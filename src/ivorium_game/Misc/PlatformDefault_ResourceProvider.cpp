#include "PlatformDefault_ResourceProvider.hpp"

namespace iv
{

PlatformDefault_ResourceProvider::PlatformDefault_ResourceProvider( Instance * inst, size_t priority ) :
    cm( inst, this, "PlatformDefault_ResourceProvider" ),
#if IV_FSPLATFORM_XDG || IV_FSPLATFORM_WIN
    provider( inst, priority, "./data/" )
#elif IV_FSPLATFORM_EMSCRIPTEN
    provider( inst, priority, "./data/" )
#elif IV_FSPLATFORM_ANDROID
    provider( inst, priority, "" )
#else
    #error "Unimplemented FS Platform."
#endif
{
    this->cm.inherits( this->provider.cm );
}

Instance * PlatformDefault_ResourceProvider::instance() const
{
    return this->provider.instance();
}

}
