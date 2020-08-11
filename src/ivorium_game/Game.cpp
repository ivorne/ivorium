#include <ivorium_config.hpp>
#if IV_FSPLATFORM_WIN
    #define WINVER 0x0600
    #define _WIN32_WINNT 0x0600
    #include <windows.h>
    #include <shlobj.h>
    #include <locale>
    #include <codecvt>
#endif

#include "Game.hpp"

#include "Misc/PlatformDefault_ResourceProvider.hpp"
#include "Misc/DefaultBinder.hpp"
#include <ivorium_systems/ivorium_systems.hpp>
#include <ivorium_graphics/ivorium_graphics.hpp>
#include <ivorium_UI/ivorium_UI.hpp>

#if IV_FSPLATFORM_ANDROID
    #include <glfm.h>
#endif

namespace iv
{

static std::string l_ConfigBase( std::string application_name )
{
#if IV_FSPLATFORM_XDG

    std::string conf_home;
    const char * xdg_config_home = std::getenv( "XDG_CONFIG_HOME" );
    if( xdg_config_home )
    {
        conf_home = xdg_config_home;
    }
    else
    {
        const char * home = std::getenv( "HOME" );
        if( home )
        {
            conf_home = home;
            conf_home += "/.config";
        }
        else
        {
            conf_home = "./.config";
        }
    }
    
    return conf_home + "/" + application_name;
    
#elif IV_FSPLATFORM_ANDROID

    #if IV_GLPLATFORM_GLFM
        return glfmAndroidGetActivity()->externalDataPath;
    #else
        #error "Can not retrieve configuration base directory."
    #endif
    
#elif IV_FSPLATFORM_EMSCRIPTEN

    return "";
    
#elif IV_FSPLATFORM_WIN

    // read local appdata path
    PWSTR localAppDataPath;
    HRESULT call_result = SHGetKnownFolderPath( FOLDERID_LocalAppData, 0, nullptr, &localAppDataPath );
    if( call_result != S_OK )
    {
        runtime_warning( SRC_INFO, "Failed to get FOLDERID_LocalAppData path." );
        return "";
    }
    std::wstring wpath = localAppDataPath;
    CoTaskMemFree( static_cast< void * >( localAppDataPath ) );
    
    // convert from wstring to string
    using convert_typeX = std::codecvt_utf8< wchar_t >;
    std::wstring_convert< convert_typeX, wchar_t > converterX;
    std::string path = converterX.to_bytes( wpath );
    
    // extend with application name and return
    return path + "/" + application_name;
    
#else
    #error "Unknown filesystem platform."
#endif
}

Game::Game( Window * window, GameIdentity const & identity ) :
    _window( window )
{
    // basic systems
    this->sc.template createOwnSystem< InstanceSystem >();
    this->sc.template createOwnSystem< DVarSystem >();
    this->sc.template createOwnSystem< DelayedLoadSystem >();
    this->sc.template createOwnSystem< TimeSystem >();
    this->sc.template createOwnSystem< ConfigFileSystem >( l_ConfigBase( identity.configuration_tag.c_str() ) );
    this->sc.template createOwnSystem< RandomSystem >();
    this->sc.template createOwnSystem< ResourceManagementSystem >();
    this->sc.template createOwnSystem< GlSystem >( this->render_target(), this->_window->gpu_enabled() );
    this->sc.template createOwnSystem< InputSystem >( window );
    this->sc.template createOwnSystem< InputBindingSystem >();
    this->sc.template createOwnSystem< ElementSystem >();
    this->sc.template createOwnSystem< AnimSystem >();
    this->sc.template createOwnSystem< LumaSystem >();
    
    // resource providers
    this->heap.template create< I< VirtualResourceProvider > >( "virtual_resources", this->system_container(), 0 );
    this->heap.template create< I< PlatformDefault_ResourceProvider > >( "stream_resources", this->system_container(), 1 );
    
    // input binding
    this->heap.template create< I< DefaultBinder > >( "default_binder", this->system_container() );
    
    //
    this->_window->set_listener( this );
}

Game::~Game()
{
    // delete all objects before systems are deinitialized
    this->heap.clear();
    
    // remove systems in right order (automatic system removal uses random order, which is not ideal)
    this->sc.template removeSystem< LumaSystem >();
    this->sc.template removeSystem< AnimSystem >();
    this->sc.template removeSystem< ElementSystem >();
    this->sc.template removeSystem< InputBindingSystem >();
    this->sc.template removeSystem< InputSystem >();
    this->sc.template removeSystem< GlSystem >();
    this->sc.template removeSystem< ResourceManagementSystem >();
    this->sc.template removeSystem< RandomSystem >();
    this->sc.template removeSystem< ConfigFileSystem >();
    this->sc.template removeSystem< TimeSystem >();
    this->sc.template removeSystem< DelayedLoadSystem >();
    this->sc.template removeSystem< DVarSystem >();
    this->sc.template removeSystem< InstanceSystem >();
}

SystemContainer * Game::system_container()
{
    return &this->sc;
}

RenderTarget::Geometry Game::geometry() const
{
    return this->_window->geometry();
}

RenderTarget * Game::render_target() const
{
    return this->_window->render_target();
}

bool Game::input( Input const * input )
{
    return this->game_input( input );
}

void Game::focus_lost()
{
    this->game_focusLost();
}

void Game::update( uint64_t delta_ms )
{
    this->system_container()->nextFrame();
    this->system_container()->getSystem< TimeSystem >()->nextFrame();
    this->system_container()->getSystem< TimeSystem >()->addTime( delta_ms );
    this->system_container()->flushSystems();
    this->system_container()->getSystem< AnimSystem >()->AnimUpdate( delta_ms );
}

void Game::draw()
{
    this->game_draw();
}

void Game::gpu( bool enabled, bool dropped )
{
    auto * gls = this->system_container()->getSystem< GlSystem >();
    
    if( dropped )
        gls->gl_drop();
    
    if( enabled )
        gls->gl_enable();
    else
        gls->gl_disable();
}

void Game::resized( RenderTarget::Geometry geometry )
{
    this->system_container()->getSystem< InputSystem >()->window_size( geometry.size );
    this->game_geometryChanged( geometry );
}

bool Game::extra_update()
{
    if( auto dls = this->system_container()->getSystem< DelayedLoadSystem >() )
        return dls->loadStep();
    
    return false;
}

}
