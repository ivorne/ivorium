#pragma once

#include "../OpenGL/RenderTarget.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <map>

namespace iv
{

class Game;

/**
*/
class GlInfo
{
public:
                        GlInfo( Instance * inst );
    Instance *          instance() const;

    bool                GlIsEnabled() const;
    RenderTarget *      render_target() const;
    
private:
    Instance * inst;
};

/**
*/
class GlListener : public GlInfo
{
public:
                        GlListener( Instance * inst );
                        ~GlListener();

    virtual void        GlEnable() = 0;
    virtual void        GlDisable() = 0;
    virtual void        GlDrop() = 0;
};

/**
*/
class GlSystem : public System
{
public:
    ///\name System control
    ///\{
                            GlSystem( SystemContainer * sc, RenderTarget * rt, bool gl_enabled );
                            ~GlSystem();
                            
    void                    gl_enable();
    void                    gl_disable();
    void                    gl_drop();
    ///\}
    
    ///\name debug
    ///\{
    /**
    */
    virtual std::string     debug_name() const override { return "GlSystem"; }
    ///\}
    
    ///\name client interface
    ///\{
    /**
        GlListener methods are called right before or right after the scene render (inside DisplaySystem::draw).
        This is used mainly on Android, on PC it may be used for some debugging or also for backgrounding.
        GlListeners should not be added or removed during the callback or during render - I expect the changes in instances to be done in reaction to data changes, not in reaction to rendering requests.
    */
    void                    AddGlListener( GlListener * listener );
    void                    RemoveGlListener( GlListener * listener );
    bool                    GlIsEnabled() const;
    
    /**
    */
    RenderTarget *          render_target() const;
    
    ///\}
    
private:
    void                    ApplyTargetGlEnabled();
    void                    ListenersEnableGl( bool enable );
    
private:
    // target
    RenderTarget * _render_target;
    
    // listeners
    bool gl_listeners_iterating;
    std::unordered_set< GlListener * > gl_listeners;
    
    // state
    bool gl_enabled;
};

}
