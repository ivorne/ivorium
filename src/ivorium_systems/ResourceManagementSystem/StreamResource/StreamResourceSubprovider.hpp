#pragma once

#include "StreamResourceProvider.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <unordered_map>

namespace iv
{

/**
*/
class StreamResourceSubprovider
{
public:
ClientMarker cm;
                            StreamResourceSubprovider( Instance * inst );
    
    Instance *              instance() const;
    
protected:
    void                    Parse_EachResource( std::istream & metadata_file );
    void                    Parse_EachParameter( std::function< void( std::string const & param, std::string const & val ) > const & = nullptr );
    
    virtual void            Resource( std::string const & resource_class, ResourcePath path ) = 0;  ///< This must call Parse_EachParameter.
    
private:
    JsonLex lex;
};

/**
*/
class Plain_StreamResourceSubprovider : public StreamResourceSubprovider
{
public:
ClientMarker cm;
                            Plain_StreamResourceSubprovider( Instance * inst, StreamResourceProvider const * provider, char const * metadata_class );
    void                    each_resource( std::function< void( ResourcePath const & ) > const & ) const;
    bool                    has_resource( ResourcePath const & ) const;
    
protected:
    virtual void            Resource( std::string const & resource_class, ResourcePath path ) override;
    
private:
    char const * metadata_class;
    std::unordered_set< ResourcePath > resources;
};

}
