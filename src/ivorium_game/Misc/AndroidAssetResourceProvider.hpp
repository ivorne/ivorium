#pragma once

#include <ivorium_systems/ivorium_systems.hpp>
#include <string>

namespace iv
{

/**
*/
class AndroidAssetResourceProvider : private StreamResourceProvider
{
public:
using StreamResourceProvider::instance;
ClientMarker cm;

                                            AndroidAssetResourceProvider( Instance * inst, size_t priority, std::string const & root_path );
    
    virtual void                            with_stream( ResourcePath const & path, std::function< void( std::istream & ) > const & ) const override;
    virtual void                            with_metadata_stream( std::function< void( std::istream & ) > const & ) const override;
    
private:
    std::string toFsPath( ResourcePath path ) const;
    
private:
    std::string root;
};

}
