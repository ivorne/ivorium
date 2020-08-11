#pragma once

#include "StreamResource/StreamResourceProvider.hpp"
#include "StreamResource/StreamResourceSubprovider.hpp"
#include "SingularResource.hpp"

#include <string>
#include <vector>

namespace iv
{

class DataStream_Subprovider : public Plain_StreamResourceSubprovider
{
public:
ClientMarker cm;
    DataStream_Subprovider( Instance * inst, StreamResourceProvider const * );
};

/**
 DataStreamResource is usualy used by virtual resources (resources constructed by VirtualResourceProvider) which use multiple files to construct themself.
*/
class DataStream : private StreamResource
{
public:
using StreamResource::instance;
ClientMarker cm;

                            DataStream( Instance * inst, StreamResourceProvider const * provider, DataStream_Subprovider const *, ResourcePath const & path );
                            
    ResourcePath            path() const;
    void                    with_stream( std::function< void( std::istream & ) > const & ) const;
};


class DataStream_Resource : public SingularResource< DataStream >
{
public:
ClientMarker cm;
    DataStream_Resource( Instance * inst, ResourcePath const & path ) :
        SingularResource< DataStream >( inst, path ),
        cm( inst, this, "DataStream_Resource" )
    {
        this->cm.inherits( this->SingularResource< DataStream >::cm );
    }
};

}
