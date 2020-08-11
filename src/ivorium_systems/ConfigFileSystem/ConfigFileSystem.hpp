#pragma once

#include "../fs.hpp"

#include <ivorium_core/ivorium_core.hpp>
#include <ivorium_config.hpp>

#include <functional>
#include <string>

namespace iv
{

class ConfigStream
{
public:
ClientMarker cm;

                        ConfigStream( Instance * inst, std::string const & name );
                        ~ConfigStream();
    
    Instance *          instance() const;
    
    virtual void        config_stream_changed() = 0;
    
    bool                stream_exists();
    void                stream_read( std::function< void( std::istream & ) > const & );
    void                stream_write( std::function< void( std::ostream & ) > const & );
    
    /**
        Should be used for logging, not for direct access.
    */
    std::string         get_filepath();
    
private:
    Instance * inst;
    std::string name;
};

/**
    Constructor uses parameter application_name, this may be used as directory name for configuration files in system configuration home.
*/
class ConfigFileSystem : public System
{
public:
    //
                                ConfigFileSystem( SystemContainer * sc, std::string const & base_dir );
    virtual std::string         debug_name() const override { return "ConfigFileSystem"; }
    virtual bool                flushSystem() override;
    virtual void                status( TextDebugView * view ) override;
    
    //
    void                        stream_add_listener( ConfigStream * listener, std::string const & name );
    void                        stream_remove_listener( ConfigStream * listener );
    
    bool                        stream_exists( std::string const & name );
    void                        stream_read( std::string const & name, std::function< void( std::istream & ) > const & );
    void                        stream_write( std::string const & name, std::function< void( std::ostream & ) > const & );
    std::string                 get_filepath( std::string const & name );
    
#if IV_CONFIG_FS_ENABLED
private:
    struct StreamListener
    {
        ConfigStream *  listener;
        std::string     name;
        fs::file_time_type timestamp;
        
        StreamListener( ConfigStream * listener, std::string const & name, fs::file_time_type timestamp ) : listener( listener ), name( name ), timestamp( timestamp ){}
    };
    
private:
    fs::path                    stream_filepath( std::string const & name );
    fs::file_time_type          stream_timestamp( std::string const & name );
    
private:
    fs::path base_dir;
    
    std::vector< StreamListener > stream_listeners;
    
    unsigned frame_id;
#endif
};

}
