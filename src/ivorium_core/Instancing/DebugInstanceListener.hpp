#pragma once

#include "ClientMarker.hpp"
#include "Instance.hpp"

namespace iv
{

//class Instance;
class ClientMarker;

class DebugInstanceListener
{
public:
                        DebugInstanceListener( Instance * inst, InstanceSystem * target );
                        ~DebugInstanceListener();
                        
    Instance *          instance() const;
    
    // instances
    virtual void        InstanceCreated( Instance * ) {}
    virtual void        InstanceDestroyed( Instance * ) {}
    
    // clients
    virtual void        ClientCreated( ClientMarker const * ) {}
    virtual void        ClientDestroyed( ClientMarker const * ) {}
    
    // logging
    virtual void        ClientLog( ClientMarker const * marker, SrcInfo const & info, LogId id, std::string const & message ) {}
    virtual bool        ClientLogEnabled( ClientMarker const * marker, LogId id ) { return false; }
    
    virtual void        SystemLog( System const *, SrcInfo const & info, LogId id, std::string const & message ){}
    virtual bool        SystemLogEnabled( System const *, LogId id ){ return false; }
    
private:
    Instance * inst;
    InstanceSystem * os;
};

}
