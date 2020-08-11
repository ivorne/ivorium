#pragma once 

#include "../Basics/LogId.hpp"
#include "../Basics/volatile_set.hpp"
#include "SystemContainer.hpp"
#include "../DebugView/TreeDebugView.hpp"
#include "../DebugView/TableDebugView.hpp"
#include <unordered_set>

namespace iv
{

class Instance;
class ClientMarker;
class DebugInstanceListener;

class InstanceSystem : public System
{
public:
                            InstanceSystem( SystemContainer * );
    
    /**
    */
    virtual std::string     debug_name() const override { return "InstanceSystem"; }
    void                    status( TextDebugView * view ) override;
    virtual bool            flushSystem() override;
    
//------------------------- logging ------------------------------------------
    void                    ClientLog( ClientMarker const * cm, SrcInfo const & info, LogId id, std::string const & message );
    bool                    ClientLogEnabled( ClientMarker const * cm, LogId id );
    
    void                    SystemLog( System const * system, SrcInfo const & info, LogId id, std::string const & message );
    bool                    SystemLogEnabled( System const * system, LogId id );
    
//------------------------- introspection ------------------------------------
    /**
    */
    void                    register_instance( Instance * );
    void                    unregister_instance( Instance * );
    
    volatile_set< DebugInstanceListener * > & debug_listeners();

    /**
    */
    void                    AddDebugListener( DebugInstanceListener * listener );
    void                    RemoveDebugListener( DebugInstanceListener * listener );

    /**
    */
    std::unordered_set< Instance * > const & Debug_Instances();
    
private:
    void DebugPrintInstance( TreeDebugView * tree, Instance * inst );

private:
    std::unordered_set< Instance * > _instances;
    volatile_set< DebugInstanceListener * > _debug_listeners;
    unsigned last_check_frame_id;
};

}

#include "SystemContainer.inl"
