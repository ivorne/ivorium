#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

template< class Listener >
class GenericListener_Index;

//========================= GenericListener ============================
/**
    \ingroup ModelTools
*/
template< class Listener >
class GenericListener
{
public:
iv::ClientMarker cm;

                            GenericListener( iv::Instance * inst, GenericListener_Index< Listener > * index );
                            ~GenericListener();
    
    iv::Instance *  instance() const;
    
    /**
        Should be called after the whole instance is properly initialized to request initial calls to notify about existing states.
    */
    void                    Initialize();
    
    /**
        Called from genericListener_Index before something is invoked on this listener - to allow us to warn user about possible missing initializations.
    */
    void                    VerifyInitialization();
    
private:
    iv::Instance * inst;
    GenericListener_Index< Listener > * index;
    bool initialized;
};

//========================= GenericListener_Index ============================
/**
    \ingroup ModelTools
*/
template< class Listener >
class GenericListener_Index
{
public:
iv::ClientMarker cm;

                            GenericListener_Index( iv::Instance * inst );
    iv::Instance *          instance() const;
    
    // called by listeners
    void                    AddListener( Listener * );
    void                    RemoveListener( Listener * );
    void                    InitializeListener( Listener * );
    
    // called by owner
    template< class ... Params >
    void                    InvokeListeners( void( Listener::* member )( Params  ... ), Params ... params );
    
    
protected:
    /**
        Called after a listener is fully initialized and needs to get up to date with current state of the listened-to system.
    */
    virtual void            Listener_Initialize( Listener * ) = 0;
    
private:
    iv::Instance * inst;
    iv::volatile_set< Listener * > listeners;
};

}

#include "GenericListener.inl"
