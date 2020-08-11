#pragma once

#include "../ResourceProvider.hpp"
#include "../ResourceManagementSystem.hpp"

#include <ivorium_core/ivorium_core.hpp>

#include <unordered_map>
#include <memory>

namespace iv
{

class StreamResourceProvider;

/**
*/
class StreamResource
{
public:
ClientMarker cm;
                    StreamResource( Instance * inst, StreamResourceProvider const * provider, ResourcePath const & path );
    Instance *      instance() const;
    void            status( iv::TableDebugView * view );
    
    void            with_resource_stream( std::function< void( std::istream & ) > const & ) const;
    ResourcePath    resource_path() const;
    
private:
    Instance * inst;
    StreamResourceProvider const * provider;
    ResourcePath path;
};

/**
*/
class StreamResourceProvider : private ResourceProvider
{
public:
using ResourceProvider::instance;
ClientMarker cm;
                                                StreamResourceProvider( Instance * inst, size_t priority );
    
    virtual void                                with_metadata_stream( std::function< void( std::istream & ) > const & ) const = 0;
    virtual void                                with_stream( ResourcePath const & path, std::function< void( std::istream & ) > const & ) const = 0;
    
    template< class TStreamSubprovider, class TClient, class TParserClient = TClient >
    static void                                 RegisterType();
    static void                                 UnregisterAllTypes();
    
private:
//------------------------- ResourceProvider -----------------------
    virtual void                                each_type( std::function< void( std::type_index type ) > const & ) override;
    virtual bool                                has_path( ResourcePath const & path, std::type_index type ) override;
    virtual void                                each_resource( std::type_index type, std::function< void( ResourcePath const & ) > const & ) override;
    virtual std::pair< void *, Instance * >     create_resource( ResourcePath const & path, std::type_index type, ResourcesRoot * parent ) override;
    
private:
    class Marker
    {
    public:
        virtual                                     ~Marker(){}
        virtual std::pair< void *, Instance * >     MakeResource( StreamResourceProvider *, ResourcesRoot *, ResourcePath const & ) const = 0;
        virtual void                                EachResource( StreamResourceProvider *, std::function< void( ResourcePath const & path ) > const & ) const = 0;
        virtual bool                                HasResource( StreamResourceProvider *, ResourcePath const & ) const = 0;
        virtual std::type_index                     GetType() const = 0;
    };
    
    template< class TStreamSubprovider, class TClient, class TParserClient >
    class TMarker : public Marker
    {
    public:
        virtual std::pair< void *, Instance * >     MakeResource( StreamResourceProvider *, ResourcesRoot *, ResourcePath const & ) const override;
        virtual void                                EachResource( StreamResourceProvider *, std::function< void( ResourcePath const & path ) > const & ) const override;
        virtual bool                                HasResource( StreamResourceProvider *, ResourcePath const & ) const override;
        virtual std::type_index                     GetType() const override;
    private:
        TStreamSubprovider *                        GetSubprovider( StreamResourceProvider * ) const;
    };
    
    static std::unordered_multimap< std::type_index, std::unique_ptr< Marker > > * Markers;
    mutable std::unordered_map< Marker const *, void * > subproviders;
    
    Heap heap;
};

}

#include "StreamResourceProvider.inl"
