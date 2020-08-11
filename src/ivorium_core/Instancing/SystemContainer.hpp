#pragma once

#include "../DebugView/TextDebugView.hpp"
#include "../Basics/SrcInfo.hpp"
#include "../Basics/static_warning.hpp"

#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <csignal>

namespace iv
{

class SystemContainer;
class TextDebugView;

class System : public Context
{
public:
                                System( SystemContainer * sc );
    virtual                     ~System(){}
    
    virtual bool                flushSystem(){ return false; }
    virtual void                status( TextDebugView * view ){}
    virtual std::string         debug_name() const{ return ""; }
    
    /**
    */
    SystemContainer *           system_container() const;
    
    /**
        System is created with zero refcounters, so it should be retained after creation.
    */
    void                        retain();
    /**
        When last reference is released, the System will delete itself (using delete operator).
    */
    void                        release();
    
protected:
    virtual bool                log_process_enabled( LogId id ) const override final;
    virtual void                log_process( SrcInfo const & info, LogId id, std::string const & message ) const override final;
    
private:
    SystemContainer * sc;
    unsigned _refcounters;
};

/**
    If multiple SystemContainers contain the same instance of a system, the system will be deleted after last of those SystemContainers is deleted, no matter which created it.
*/
class SystemContainer
{
public:
    /**
     * Creates empty SystemContainer.
     */
                                SystemContainer();
                                
    /**
     * Copies pointers to systems from given SystemContainer.
     */
                                SystemContainer( SystemContainer const * sc );
                                
    /**
     * Deallocates all owned systems.
     */
                                ~SystemContainer();
    
    void                        debug_print( TextDebugView * view ) const;
    
    /**
        Only for debug purposes (essentialy just to call System::debug_print on them).
    */
    std::unordered_map< std::type_index /*type*/, System * > const & debug_GetSystems() const;
    
    /**
     * System::flushSystem will be called on all systems repeatedly until all
     * systems return false from the flush.
     * This guarantees that all systems will do all their updates and everything
     * will be in stable state (no need for any more updates until next frame).
     */
    bool                        flushSystems();
    
    /**
    */
    void                        nextFrame();
    
    /**
    */
    unsigned                    frame_id() const;
    
    /**
     * Returns system that was inserted into SystemContainer with the same type.
     */
    template< class TypedSystem >
    TypedSystem *               getSystem() const;
    
    /**
        Same as templated version but needs just std::type_index and not compile-time type of system and returns only interface of the system.
    */
    System *                    getSystem( std::type_index type ) const;
    
    /**
     * Create system that will be owned by this SystemContainer.
     * If foreign system exists in this SC, then it will be unlinked and new own will be created.
     * If own system already exists, it will be just returned and \p constructor_arguments will not be used.
     */
    template< class TypedSystem, typename... Args >
    TypedSystem *               createOwnSystem( Args... constructor_arguments );
    
    /**
     * Add system to this SystemContainer.
     * This is used mainly when cloning SystemContainer to let it use the same systems as parent uses.
     * If this system (either own or foreign) exists, nothing will be changed and current system will be returned;
     */
    template< class TypedSystem >
    TypedSystem *               addForeignSystem( TypedSystem * system  );
    
    /**
     * Removes system from the SystemContainer.
     * If the system was owned by this SystemContainer, then it will be destroyed.
     */
    template< class TypedSystem >
    void                        removeSystem();
    
private:
    std::unordered_map< std::type_index /*type*/, System * > systems;
    std::unordered_set< System * > own_systems;
    unsigned _frame_id;
};
}
