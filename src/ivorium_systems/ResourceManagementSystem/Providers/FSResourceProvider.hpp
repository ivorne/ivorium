#pragma once

#include "../StreamResource/StreamResourceProvider.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <string>

namespace iv
{

/**
*/
class FSResourceProvider : private StreamResourceProvider
{
public:
using StreamResourceProvider::instance;
ClientMarker cm;

                                            FSResourceProvider( Instance * inst, size_t priority, const char * root_dir );
    
    // StreamResourceProvider
    virtual void                            with_stream( ResourcePath const & path, std::function< void( std::istream & ) > const & ) const override;
    virtual void                            with_metadata_stream( std::function< void( std::istream & ) > const & ) const override;
    
private:
    std::string toFsPath( ResourcePath path ) const;
    
private:
    std::string root;
};

}
