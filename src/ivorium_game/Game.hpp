#pragma once

#include <ivorium_core/ivorium_core.hpp>
#include <ivorium_graphics/ivorium_graphics.hpp>

namespace iv
{

struct GameIdentity
{
    std::string configuration_tag;
};

class Game : protected WindowListener
{
public:
                                Game( Window * window, GameIdentity const & identity );
                                ~Game();
    
    SystemContainer *           system_container();
    RenderTarget::Geometry      geometry() const;
    RenderTarget *              render_target() const;
    
protected:
    // Game
    virtual void                game_geometryChanged( RenderTarget::Geometry geometry ) = 0;
    virtual void                game_draw() = 0;                            ///< Rendering.
    virtual void                game_focusLost() = 0;                       ///< Application lost focus, meaning mostly input focus. Game should get automatically paused.
    virtual bool                game_input( Input const * input ) = 0;      ///< Returns true if input was processed (used for example on emscripten to determine if input was consumed by application or should be passed to web browser for further processing).
    
    // WindowListener
    virtual bool                input( Input const * input ) override;
    virtual void                focus_lost() override;
    virtual void                update( uint64_t delta_ms ) override;
    virtual bool                extra_update() override;
    virtual void                draw() override;
    virtual void                gpu( bool enabled, bool dropped ) override;
    virtual void                resized( RenderTarget::Geometry ) override;
    
private:
    Window *        _window;
    SystemContainer sc;
    Heap            heap;
};

/*
class Game
{
public:
                        Game( InputSource * input_source )                  : _is( input_source ){}
    virtual             ~Game(){}
    
    InputSource *       input_source()                                      { return this->_is; }

    //----------------- Game -------------------
    ///\name Game
    ///\{
    virtual void        game_start()                                            {}      ///< Called after first draw. We can draw splash screen in first draw and then load everything (or start loading it) in game_start.
    virtual bool        game_input( const Input * input )                       { return false; }   ///< \returns True if input was processed by someone.
    virtual void        game_focus_lost()                                       {}      ///< Gameplay or anything that depends on realtime player interaction should be paused.
    virtual void        game_update( uint64_t delta_ms )                        {}
    virtual void        game_draw()                                             {}
    virtual void        game_exit()                                             {}      ///< Called before destructor. May make it a bit easier to do cleanup.
    
    virtual void        game_geometry( RenderTarget::Geometry geometry )        {}
    virtual void        game_gpu( bool enabled, bool dropped )                  {}
    ///\}
    
private:
    InputSource * _is;
};
*/

}
