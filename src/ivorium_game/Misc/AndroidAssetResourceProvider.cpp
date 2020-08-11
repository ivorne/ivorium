#include "AndroidAssetResourceProvider.hpp"

#if IV_ENABLE_ANDROID_JNI

#include <ivorium_core/ivorium_core.hpp>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <glfm.h>
#include <array>

#include <fstream>

namespace iv
{

class AndroidAsset_streambuf : public std::streambuf
{
public:
    static constexpr int const BufferCapacity = 128;

    AndroidAsset_streambuf( AAsset * asset, ClientMarker const * cm ) :
        cm( cm ),
        asset( asset ),
        buffer(),
        bufsize( 0 ),
        bufpos( 0 )
    {
    }
    
    ~AndroidAsset_streambuf()
    {
    }
    
    virtual int underflow() override
    {
        // read data to buffer
        int received = AAsset_read( this->asset, this->buffer.data(), AndroidAsset_streambuf::BufferCapacity );
        
        // check data status
        if( received < 0 )
            this->cm->warning( SRC_INFO, "Failed to read android asset data (errcode ", received, ")." );
        
        if( received <= 0 )
            return traits_type::eof();
        
        // assign buffer
        setg( this->buffer.data(), this->buffer.data(), this->buffer.data() + received );
   
        // return success
        return traits_type::to_int_type( *gptr() );
    }
    
private:
    ClientMarker const * cm;
    AAsset * asset;
    
    std::array< char, AndroidAsset_streambuf::BufferCapacity > buffer;
    int bufsize;
    int bufpos;
};

class AndroidAsset_istream : public std::istream
{
public:
    AndroidAsset_istream( AAsset * asset, ClientMarker const * cm ) :
        std::istream( new AndroidAsset_streambuf( asset, cm ) )
    {
    }
    
    ~AndroidAsset_istream()
    {
        delete this->rdbuf();
    }
};

static AAssetManager * l_GetAndroidAssetManager()
{
    return glfmAndroidGetActivity()->assetManager;
}

AndroidAssetResourceProvider::AndroidAssetResourceProvider( Instance * inst, size_t priority, std::string const & root_path ) :
    StreamResourceProvider( inst, priority ),
    cm( inst, this, "AndroidAssetResourceProvider" ),
    root( root_path )
{
    this->cm.inherits( this->StreamResourceProvider::cm );
}

void AndroidAssetResourceProvider::with_stream( ResourcePath const & path, std::function< void( std::istream & ) > const & f ) const
{
    LambdaLogTrace _log_trace(
        [&]( std::ostream & out )
        {
            out << "Android asset '" << this->toFsPath( path ) << "':" << std::endl;
        }
    );
    
    // get input filename
    //std::string rpath = this->toFsPath( path );
    std::string rpath = path.string();
    if( rpath.size() > 0 )
        rpath.erase( 0, 1 );
    
    // open stream
    AAsset * asset = AAssetManager_open( l_GetAndroidAssetManager(), rpath.c_str(), AASSET_MODE_STREAMING );
    
    if( !asset )
    {
        this->cm.warning( SRC_INFO, "Failed to open android asset '", rpath, "'." );
        return;
    }
    
    AndroidAsset_istream in( asset, &this->cm );
    
    f( in );
    
    AAsset_close( asset );
}

void AndroidAssetResourceProvider::with_metadata_stream( std::function< void( std::istream & ) > const & f ) const
{
    this->with_stream( ResourcePath( "/metadata.json" ), f );
}

/*
static void PrintDir( AAssetManager * asset_manager, const char * dirpath )
{
    auto dir = AAssetManager_openDir( asset_manager, dirpath );
    if( dir )
    {
        TextOutput << "AndroidAssetResourceProvider: dir '"<<dirpath<<"': " << endl;
        while( auto name = AAssetDir_getNextFileName( dir ) )
            TextOutput << "    '" << name << "'" << endl;
        AAssetDir_close( dir );
    }
    else
    {
        TextOutput << "AndroidAssetResourceProvider: dir '"<<dirpath<<"': <failed to open>" << endl;
    }
}
*/
/*
bool AndroidAssetResourceProvider::has_stream_path( ResourcePath const & path )
{
    // join paths
    std::string rpath = this->toFsPath( path );
    
    // try to open it
    AAsset * asset = AAssetManager_open( l_GetAndroidAssetManager(), rpath.c_str(), AASSET_MODE_STREAMING );

    if( !asset )
        return false;
    
    AAsset_close( asset );
    
    return true;
}
*/
std::string AndroidAssetResourceProvider::toFsPath( ResourcePath path ) const
{
    return path.to_real_path( this->root.c_str() );
}

}

#endif
