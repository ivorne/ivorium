#include "FSResourceProvider.hpp"
#include "../ResourcePath.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <fstream>

namespace iv
{

FSResourceProvider::FSResourceProvider( Instance * inst, size_t priority, const char * root_dir ) :
    StreamResourceProvider( inst, priority ),
    cm( inst, this, "FSResourceProvider" ),
    root( root_dir )
{
    this->cm.inherits( this->StreamResourceProvider::cm );
}

void FSResourceProvider::with_stream( ResourcePath const & path, std::function< void( std::istream & ) > const & f ) const
{
    LambdaLogTrace _log_trace(
        [&]( std::ostream & out )
        {
            out << "File '" << this->toFsPath( path ) << "':" << std::endl;
        }
    );
    
    // get input filename
    std::string rpath = this->toFsPath( path );
    
    // open stream
    std::ifstream in( rpath.c_str(), std::ios::binary );
    if( !in.good() )
    {
        this->cm.warning( SRC_INFO, "Can not properly open resource file '", rpath, "'." );
        return;
    }
    
    //
    f( in );
    
    //
    #warning "TODO - Maybe check if the the stream is valid."
    /*if( in.fail() || in.bad() )
    {
        this->instance()->warning( SRC_INFO, Warning( "Reading stream failed: ",strerror(errno),"." ) );
        return;
    }*/
}

void FSResourceProvider::with_metadata_stream( std::function< void( std::istream & ) > const & f ) const
{
    this->with_stream( ResourcePath( "/metadata.json" ), f );
}

std::string FSResourceProvider::toFsPath( ResourcePath path ) const
{
    return path.to_real_path( this->root.c_str() );
}

}
