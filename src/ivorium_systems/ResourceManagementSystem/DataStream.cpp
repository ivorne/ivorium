#include "DataStream.hpp"

namespace iv
{

DataStream_Subprovider::DataStream_Subprovider( Instance * inst, StreamResourceProvider const * provider ) :
    Plain_StreamResourceSubprovider( inst, provider, "data" ),
    cm( inst, this, "DataStream_Subprovider" )
{
    this->cm.inherits( this->Plain_StreamResourceSubprovider::cm );
}

DataStream::DataStream( Instance * inst, StreamResourceProvider const * provider, DataStream_Subprovider const *, ResourcePath const & path ) :
    StreamResource( inst, provider, path ),
    cm( inst, this, "DataStream" )
{
    this->cm.inherits( this->StreamResource::cm );
}

ResourcePath DataStream::path() const
{
    return this->resource_path();
}

void DataStream::with_stream( std::function< void( std::istream & ) > const & f ) const
{
    Resource_LogTrace _trace( this->resource_path() );
    this->with_resource_stream( f );
}

}
