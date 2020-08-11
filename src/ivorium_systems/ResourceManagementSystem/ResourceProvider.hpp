#pragma once

#include <ivorium_core/ivorium_core.hpp>
#include <typeindex>
#include <unordered_map>
#include <functional>

namespace iv
{

class ResourcesRoot;
class ResourceManagementSystem;
class Instance;
class ResourcePath;

/**
    Should be able to register self either for all types or for just some specific types.
    Higher priority providers go first.
*/
class ResourceProvider
{
public:
ClientMarker cm;

                            ResourceProvider( Instance * inst, size_t priority );
                            ~ResourceProvider();
    Instance *              instance() const;

    /**
        First check for ability to create instance.
        If this returns true, instance creation might still fail (parsing error) but no other provider will try to instantiate it.
        Only resource provider with highest priority that returns true here will try to instantiate it.
        This ResourceProvider must not be removed from ResourceManagementSystem during this method call.
    */
    virtual bool            has_path( ResourcePath const & path, std::type_index type ) = 0;
    
    /**
        Is not called if create_instance_async was called on the same path and type before.
        Should return void pointer to instance of type indentified by argument 'type' and its associated Instance that owns it (for memory management).
    */
    virtual std::pair< void *, Instance * > create_resource( ResourcePath const & path, std::type_index type, ResourcesRoot * parent ) = 0;
    
    /**
    */
    virtual void            each_resource( std::type_index type, std::function< void( ResourcePath const & ) > const & ) = 0;
    
    /**
    */
    virtual void            each_type( std::function< void( std::type_index type ) > const & ) = 0;
    
protected:
    /**
        Call this to unregister from ResourceManagementSystem prematurely.
        This does not have to be called, it will eventualy be called from ResourceProvider::~ResourceProvider.
    */
    void                    unregister_provider();
    
private:
    Instance *                  inst;
    ResourceManagementSystem *  rms;
};

}

