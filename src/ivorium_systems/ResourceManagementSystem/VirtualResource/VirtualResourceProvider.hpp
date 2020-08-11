#pragma once

#include "../ResourceProvider.hpp"
#include "../ResourceManagementSystem.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <memory>

namespace iv
{

/**
 If the path exists dependends on the resource implementation.
 This can be used for example if whole directory is a resource or just part of the path.
*/
class VirtualResourceProvider : private ResourceProvider
{
public:
using ResourceProvider::instance;
ClientMarker cm;

                                                VirtualResourceProvider( Instance * inst, size_t priority );
    
    template< class TVirtualSubprovider, class TClient, class TParserClient = TClient >
    static void                                 RegisterType();
    static void                                 UnregisterAllTypes();
    
//--------------------------------------------- ResourceProvider -----------------------
    virtual void                                each_type( std::function< void( std::type_index type ) > const & ) override;
    virtual bool                                has_path( ResourcePath const & path, std::type_index type ) override;
    virtual void                                each_resource( std::type_index type, std::function< void( ResourcePath const & ) > const & ) override;
    virtual std::pair< void *, Instance * >     create_resource( ResourcePath const & path, std::type_index type, ResourcesRoot * parent ) override;
    
private:
    class Marker
    {
    public:
        virtual                                     ~Marker(){}
        virtual std::pair< void *, Instance * >     MakeResource( VirtualResourceProvider *, ResourcesRoot *, ResourcePath const & ) const = 0;
        virtual void                                EachResource( VirtualResourceProvider *, std::function< void( ResourcePath const & path ) > const & ) const = 0;
        virtual bool                                HasResource( VirtualResourceProvider *, ResourcePath const & ) const = 0;
        virtual std::type_index                     GetType() const = 0;
    };
    
    template< class TVirtualSubprovider, class TClient, class TParserClient >
    class TMarker : public Marker
    {
    public:
        virtual std::pair< void *, Instance * >     MakeResource( VirtualResourceProvider *, ResourcesRoot *, ResourcePath const & ) const override;
        virtual void                                EachResource( VirtualResourceProvider *, std::function< void( ResourcePath const & path ) > const & ) const override;
        virtual bool                                HasResource( VirtualResourceProvider *, ResourcePath const & ) const override;
        virtual std::type_index                     GetType() const override;
    private:
        TVirtualSubprovider *                       GetSubprovider( VirtualResourceProvider * ) const;
    };
    
    static std::unordered_multimap< std::type_index, std::unique_ptr< Marker > > * Markers;
    mutable std::unordered_map< Marker const *, void * > subproviders;
    
    Heap heap;
};

}

#include "VirtualResourceProvider.inl"
