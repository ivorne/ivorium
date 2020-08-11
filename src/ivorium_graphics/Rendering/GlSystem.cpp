#include "GlSystem.hpp"

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

GlSystem::GlSystem( SystemContainer * sc, RenderTarget * rt, bool gl_enabled ) :
    System( sc ),
    _render_target( rt ),
    gl_listeners_iterating( false ),
    gl_enabled( gl_enabled )
{
}

GlSystem::~GlSystem()
{
}

RenderTarget * GlSystem::render_target() const
{
    return this->_render_target;
}

void GlSystem::gl_enable()
{
    if( this->gl_enabled )
        return;
    
    this->gl_enabled = true;
    
    this->gl_listeners_iterating = true;
    for( auto listener : this->gl_listeners )
        listener->GlEnable();
    this->gl_listeners_iterating = false;
}

void GlSystem::gl_disable()
{
    if( !this->gl_enabled )
        return;
    
    this->gl_listeners_iterating = true;
    for( auto listener : this->gl_listeners )
        listener->GlEnable();
    this->gl_listeners_iterating = false;
    
    this->gl_enabled = false;
}

void GlSystem::gl_drop()
{
    if( !this->gl_enabled )
        return;
    
    this->gl_enabled = false;
    
    this->gl_listeners_iterating = true;
    for( auto listener : this->gl_listeners )
        listener->GlDrop();
    this->gl_listeners_iterating = false;
}

bool GlSystem::GlIsEnabled() const
{
    return this->gl_enabled;
}

void GlSystem::AddGlListener( GlListener * listener )
{
    if( this->gl_listeners_iterating )
        this->warning( SRC_INFO, "Can not add gl listener when notifying gl listeners," );
    
    this->gl_listeners.insert( listener );
}

void GlSystem::RemoveGlListener( GlListener * listener )
{
    if( this->gl_listeners_iterating )
        this->warning( SRC_INFO, "Can not add gl listener when notifying gl listeners," );
    
    this->gl_listeners.erase( listener );
}

GlInfo::GlInfo( Instance * inst ) :
    inst( inst )
{
}

Instance * GlInfo::instance() const
{
    return this->inst;
}

bool GlInfo::GlIsEnabled() const
{
    auto ds = this->instance()->getSystem< GlSystem >();
    if( !ds )
        return false;
    
    return ds->GlIsEnabled();
}

RenderTarget * GlInfo::render_target() const
{
    auto ds = this->instance()->getSystem< GlSystem >();
    if( !ds )
        return nullptr;
    
    return ds->render_target();;
}

GlListener::GlListener( Instance * inst ) :
    GlInfo( inst )
{
    auto ds = this->instance()->getSystem< GlSystem >();
    if( !ds )
        return;
    
    ds->AddGlListener( this );
}

GlListener::~GlListener()
{
    auto ds = this->instance()->getSystem< GlSystem >();
    if( !ds )
        return;
    
    ds->RemoveGlListener( this );
}

}
