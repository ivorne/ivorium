#pragma once

#include "ResourceProvider.hpp"
#include "ResourcePath.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <map>

namespace iv
{

class ResourcesRoot
{
public:
ClientMarker cm;
    ResourcesRoot( Instance * inst ):
        cm( inst, this, "ResourcesRoot" ),
        inst( inst )
    {
    }
    
    Instance * instance()
    {
        return this->inst;
    }
    
    Heap heap;
    
private:
    Instance * inst;
};


/**
    Helps to use shared and streamed resources and abstracts file access (file system, zip file, net sync, manualy created instances, ...).
    Each resource is identified by its type (c++ class) and virtual file path.
*/
class ResourceManagementSystem : public System
{
public:
                            ResourceManagementSystem( SystemContainer * sc );
                            ~ResourceManagementSystem();
    virtual std::string     debug_name() const override { return "ResourceManagementSystem"; }
    virtual void            status( TextDebugView * view ) override;
    
//------- resource requests --------
    template< class TResource >
    bool                has_path( ResourcePath path );

    /**
    */
    template< class TResource >
    TResource const *   get( ResourcePath path );

    template< class TResource >
    void                each_resource( std::function< void( ResourcePath const & path ) > const & );
    
//------ provider registration ------
    /**
        Usage of providers has linear complexity, if a high number of providers would be used, this should be changed.
        If more providers have the same priority, then their order is undefined.
        
        Provider registration and unregistration might be called from provider constructor / destructor, so virtual methods of provider should not be called right from this methods.
    */
    void                register_provider( ResourceProvider * provider, size_t priority );
    void                unregister_provider( ResourceProvider * provider );

private:
    ResourceProvider *  choose_provider( ResourcePath const & path, std::type_index type );
    
private:
    I< ResourcesRoot > root;

// provider data
    using PriorityCmp = std::greater< size_t >;                                             ///< Higher priority first.
    std::multimap< size_t /*priority*/, ResourceProvider *, PriorityCmp > providers;        ///< List of providers ordered by priority.
    
// resource data
    struct Resource
    {
        Resource() : type( typeid( void ) ), client( nullptr ), inst( nullptr ){}
        
        std::type_index type;
        void * client;
        Instance * inst;
    };

    std::unordered_multimap< ResourcePath, Resource > resources;
};

}

#include "ResourceManagementSystem.inl"
