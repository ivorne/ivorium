#include "ConfigFileSystem.hpp"
#include <fstream>

namespace iv
{

//================================================================================
//--------------------- ConfigStream ---------------------------------------------
ConfigStream::ConfigStream( Instance * inst, std::string const & name ) :
    cm( inst, this, "ConfigStream" ),
    inst( inst ),
    name( name )
{
    auto cfs = this->instance()->getSystem< ConfigFileSystem >();
    if( cfs )
        cfs->stream_add_listener( this, this->name );
}

ConfigStream::~ConfigStream()
{
    auto cfs = this->instance()->getSystem< ConfigFileSystem >();
    if( cfs )
        cfs->stream_remove_listener( this );
}

Instance * ConfigStream::instance() const
{
    return this->inst;
}

bool ConfigStream::stream_exists()
{
    auto cfs = this->instance()->getSystem< ConfigFileSystem >();
    if( cfs )
        return cfs->stream_exists( this->name );
    return false;
}

void ConfigStream::stream_read( std::function< void( std::istream & ) > const & f )
{
    auto cfs = this->instance()->getSystem< ConfigFileSystem >();
    if( cfs )
    {
        LambdaLogTrace _trace(
            [ this ]( std::ostream & out )
            {
                out << "Configuration  '"<< this->name <<"'" << std::endl;
            }
        );
        
        cfs->stream_read( this->name, f );
    }
}

void ConfigStream::stream_write( std::function< void( std::ostream & ) > const & f )
{
    auto cfs = this->instance()->getSystem< ConfigFileSystem >();
    if( cfs )
        cfs->stream_write( this->name, f );
}

std::string ConfigStream::get_filepath()
{
    auto cfs = this->instance()->getSystem< ConfigFileSystem >();
    if( cfs )
        return cfs->get_filepath( this->name );
    return "";
}

//================================================================================
//------------------------- ConfigFileSystem -------------------------------------
#if IV_CONFIG_FS_ENABLED

ConfigFileSystem::ConfigFileSystem( SystemContainer * sc, std::string const & base_dir ) :
    System( sc ),
    base_dir( base_dir ),
    frame_id( 0 )
{
}

bool ConfigFileSystem::flushSystem()
{
    // check if this is a new frame
    if( !this->system_container() )
        return false;
    
    if( this->frame_id == this->system_container()->frame_id() )
        return false;
    
    this->frame_id = this->system_container()->frame_id();
    
    // stream listeners
    for( StreamListener & listener : this->stream_listeners )
    {
        auto timestamp = this->stream_timestamp( listener.name );
        if( timestamp > listener.timestamp )
        {
            listener.timestamp = timestamp;
            listener.listener->config_stream_changed();
        }
    }
    
    //
    return true;
}

void ConfigFileSystem::status( TextDebugView * context )
{
    context->out() << "Configuration path: " << this->base_dir << "." << std::endl;
    
    for( StreamListener const & listener : this->stream_listeners )
    {
        context->out() << "Stream   '"<< listener.name <<"':" << std::endl;
        context->prefix_push( "    " );
        
        context->out() << "filepath   " << this->stream_filepath( listener.name ) << std::endl;
        context->out() << "exists     " << ( fs::is_regular_file( this->stream_filepath( listener.name ) ) ? "true" : "false" ) << std::endl;
        
        if( listener.timestamp == fs::file_time_type::min() )
        {
            context->out() << "timestamp  -" << std::endl;
        }
        else
        {
            std::time_t t = fs::file_time_type::clock::to_time_t( listener.timestamp );
            context->out() << "timestamp  " << std::asctime( std::localtime( &t ) ) << std::endl;
        }
        
        context->prefix_pop();
    }
}

fs::path ConfigFileSystem::stream_filepath( std::string const & name )
{
    //return ( this->base_dir / name ).lexically_normal();
    return this->base_dir / name;
}

fs::file_time_type ConfigFileSystem::stream_timestamp( std::string const & name )
{
    std::error_code ec;
    auto result = fs::last_write_time( this->stream_filepath( name ), ec );
    if( ec )
    {
        this->warning( SRC_INFO, "Can not determine last write time for file '", this->stream_filepath( name ), "'. ", 
                                       "Error code: ", ec.value(), " - '", ec.message(), "'." );
        return fs::file_time_type::min();
    }
    
    return result;
}

void ConfigFileSystem::stream_add_listener( ConfigStream * listener, std::string const & name )
{
    this->stream_listeners.push_back( StreamListener( listener, name, this->stream_timestamp( name ) ) );
}

void ConfigFileSystem::stream_remove_listener( ConfigStream * listener )
{
    for( size_t i = 0; i < this->stream_listeners.size(); i++ )
        if( this->stream_listeners[ i ].listener == listener )
        {
            this->stream_listeners.erase( this->stream_listeners.begin() + i );
            break;
        }
}

void ConfigFileSystem::stream_read( std::string const & name, std::function< void( std::istream & ) > const & f )
{
    fs::path fp = this->stream_filepath( name );
    TextOutput << "ConfigStream: reading '" << fp << "'" << std::endl;
    
    std::ifstream in( fp.c_str(), std::ios_base::in | std::ios_base::binary );
    if( !in.good() )
    {
        this->stream_write( name, []( std::ostream & ){} );
        in = std::ifstream( fp.c_str(), std::ios_base::in | std::ios_base::binary );
    }
    
    if( in.good() )
        f( in );
}

bool ConfigFileSystem::stream_exists( std::string const & name )
{
    fs::path fp = this->stream_filepath( name );
    std::ifstream in( fp.c_str(), std::ios_base::in | std::ios_base::binary );
    return in.good();
}

void ConfigFileSystem::stream_write( std::string const & name, std::function< void( std::ostream & ) > const & f )
{
    // construct filename
    fs::path fp = this->stream_filepath( name );
    fs::path basedir = fp;
    basedir.remove_filename();
    
    // create dir
    std::error_code ec;
    fs::create_directories( basedir, ec );
    if( ec )
    {
        this->warning( SRC_INFO, "Failed to create directories for config stream file. ",
                                       "Directory is '", basedir, "', error code is ", ec.value(), " - '", ec.message(), "'." );
        return;
    }
    
    // open file
    std::ofstream out( fp, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );
    if( !out.good() )
    {
        this->warning( SRC_INFO, "Can not open configuration file '", fp, "' for writing." );
        return;
    }
    
    // write to file
    f( out );
}

std::string ConfigFileSystem::get_filepath( std::string const & name )
{
    return this->stream_filepath( name ).string();
}

#else

ConfigFileSystem::ConfigFileSystem( SystemContainer * sc, std::string const & base_dir ) :
    System( sc )
{
}

void ConfigFileSystem::status( TextDebugView * context )
{
}

bool ConfigFileSystem::flushSystem()
{
    return false;
}

void ConfigFileSystem::stream_add_listener( ConfigStream * listener, std::string const & name )
{
}

void ConfigFileSystem::stream_remove_listener( ConfigStream * listener )
{
}

bool ConfigFileSystem::stream_exists( std::string const & name )
{
    return false;
}

void ConfigFileSystem::stream_read( std::string const & name, std::function< void( std::istream & ) > const & )
{
}

void ConfigFileSystem::stream_write( std::string const & name, std::function< void( std::ostream & ) > const & )
{
}

std::string ConfigFileSystem::get_filepath( std::string const & name )
{
    return "";
}

#endif

}
