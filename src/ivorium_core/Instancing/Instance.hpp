#pragma once

#include "../Basics/LogId.hpp"
#include "../Basics/SrcInfo.hpp"
#include "../Basics/volatile_set.hpp"
#include "../Basics/StringIO_defs.hpp"

#include <unordered_set>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>
#include <functional>
#include <typeindex>
#include <list>
#include <type_traits>

namespace iv
{

class InstanceSystem;
class DebugInstanceListener;
class SystemContainer;
class instance_ptr_interface;
class ClientMarker;
class TextDebugView;

/**
 */
class Instance
{
public:
    //------------------------- structors ------------------------------
                                Instance( const SystemContainer * sc );
    virtual                     ~Instance( );
    Instance *                  instance(){ return this; }
    
    
    //------------------------- SystemContainer manipulation -----------
    /**
     * Returns SystemContainer given in constructor.
     * Do not use it directly, only Properties and Components should use this to communicate with systems.
     */
    SystemContainer const *     getSystemContainer() const;
    
    /**
     * SystemContainer of this Instance will be duplicated (to contain the same systems as the original SystemContainer).
     * From now on, all children of this Instance will receive the new SystemContainer in the constructor.
     * This Instance will still use the old SystemContainer.
     * If the SystemContainer is already duplicated, this will just return pointer to it.
     * Instance can change which systems are contained in the new SystemContainer (using the pointer returned from this method).
     * 
     * Call SystemContainer::createProxies after/if you create new systems in that SystemContainer.
     */
    SystemContainer *           duplicateSystemContainer();
    
    /**
        Shortcut for getSystemContainer()->getSystem< TypedSystem >().
    */
    template< class TypedSystem >
    TypedSystem *               getSystem() const;
    
    /**
    */
    unsigned                    frame_id() const;
    
    //------------------------- instance introspection --------------------------
    /**
        Name for introspection purposes.
    */
    std::string const &         instance_name();
    
    /**
        Creates two way relation that is automaticaly cleaned when either Instance is destroyed.
        Instance can has only one parent, can be nullptr to clear existing relation.
    */
    void                        instance_parent( Instance * parent );
    
    /**
        Usually assigned right after instantiation.
        Can be called only once.
        This finalizes the Instance instanciation, so the instance is registered to InstanceSystem and debug listeners from this method.
    */
    void                        instance_finalize( std::string const & inst_name, ClientMarker const * root_client );
    
    /**
        Called automatically from ClientMarker.
    */
    void                        client_register( ClientMarker const * marker );
    void                        client_unregister( ClientMarker const * marker );
    
    //------------------------- logging --------------------------------
    void                        client_log( ClientMarker const * cm, SrcInfo const & info, LogId id, std::string const & message );
    bool                        client_log_enabled( ClientMarker const * cm, LogId id );
    
    //------------------------- debug access ---------------------------------
    void                        debug_print_clients( TextDebugView * view );
    
    /**
     */
    Instance *                                          Debug_Parent();
    std::unordered_set< Instance * > const &            Debug_Children();
    
    ClientMarker const *                                Debug_RootClient(); 
    std::unordered_set< ClientMarker const * > const &  Debug_Clients();
    
    /**
        Helper method, iterates over set returned by DebugClientMarkers(), checks type and casts client if type is equal.
        NOTE - currently unsafe - changes to Instance clients (adding or removing) may corrupt iterators used in this method and cause segfault
            - should be refactored in future to support some kind of safe listener based approach
    */
    template< class TypedClient >
    void                                            Debug_ForeachClient( std::function< void( TypedClient * ) > const & lambda );
    
private:
    InstanceSystem *            getOS();
    
private:
    //------------------ systems -------------------------------
    SystemContainer const *     sc;
    SystemContainer *           sc_dup;
    
    InstanceSystem *            os;
    
    //----------------- structure introspection -------------------------------
    std::string                         _instance_name;
    
    Instance *                          _parent;
    std::unordered_set< Instance * >    _children;
    
    ClientMarker const *                        _root_client;
    std::unordered_set< ClientMarker const * >  _clients;
    
    //------------- debug ----------------------
    int                         _logging_cnt;
    
    //------------- weak pointer impl ----------
    friend class instance_ptr_interface;
    std::unordered_set< instance_ptr_interface * > instance_pointers;
};

}

#include "SystemContainer.hpp"
#include "InstanceSystem.hpp"
#include "ClientMarker.hpp"

#include "Instance.inl"
