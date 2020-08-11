#pragma once

#include <functional>
#include <deque>
#include <typeindex>
#include <unordered_set>
#include <vector>

#include "../Basics/StringIO.hpp"
#include "../Basics/multiline_ostream.hpp"
#include "../Basics/LogId.hpp"
#include "../DebugView/TreeDebugView.hpp"
#include "../DebugView/TableDebugView.hpp"

namespace iv
{

class Instance;

/**
    If client has status printer member function and is enabled in ClientMarker constructor - ClientMarker( inst, this, "<name>", ClientMarker::Status() ), then it will be used.
    Signature of status printer:
        void client_status( iv::TableDebugContext * context );
*/
class ClientMarker : public Context
{
public:
    struct Status{};
    
public:
//========================= used by client ========================
                            template< class ClientType >
                            ClientMarker( Instance * inst, ClientType * client, const char * name );
                            
                            template< class ClientType >
                            ClientMarker( Instance * inst, ClientType * client, const char * name, Status );
                            
                            ~ClientMarker();
    
                            ClientMarker( ClientMarker const & ) = delete;
    ClientMarker &          operator=( ClientMarker const & ) = delete;
    
    /**
        Marks client relation - inheritance.
        Optional - stored only to generate nicer logs and debug outputs.
    */
    template< class ... Rest >
    void                    inherits( ClientMarker & parent, Rest & ... rest );
    
    /**
        Marks client relation - ownership.
        Optional - stored only to generate nicer logs and debug outputs.
    */
    template< class ... Rest >
    void                    owns( ClientMarker & ownee, Rest & ... rest );
    
//========================= used from the outside ====================================
    //--------- client access ------------
    /**
        Typeindex of client marked by this marker.
    */
    std::type_index         client_type() const;
    std::type_index         type() const;           ///< Alias for client_type.
    
    /**
    */
    template< class TypedClient >
    TypedClient *           client() const;
    Instance *              instance() const;
    
    //----------------
    std::string const &     name() const;
    unsigned                marker_id() const;      ///< Id unique to ClientMarker's name.
    std::string             name_id() const;        ///< Name concatenated with marker_id with space inbetween.
    std::string             root_name_id() const;   ///< Name concatenated with marker_id with space inbetween.
    
    //------ status -------
    void                    print_status( TableDebugView * view ) const;
    void                    print_status( TextDebugView * view ) const;
    
    void                    print_status_with_inherited( TextDebugView * view, ClientMarker * emphasize = nullptr ) const;
    void                    print_status_with_inherited( TreeDebugView * view, ClientMarker * emphasize = nullptr ) const;
    
    void                    print_status_with_related( TextDebugView * view ) const;
    void                    print_status_with_related( TreeDebugView * view, TreeDebugView::BoxStyle = TreeDebugView::BoxStyle::Solid ) const;
    
    //--- CM relations ------
    ClientMarker const *        inheritance_root() const;       ///< instance of ClientMarker who has no inheritance child and is descendant of this ClientMarker
    ClientMarker const *        inheritance_child() const;      ///< client that directly inherits this client as inheritance parent in this instance (or null if none does it)
    std::unordered_set< ClientMarker const * > inheritance_parents() const; ///< Clients directly inherited by this client.
    
    ClientMarker const *        relation_root() const;          ///< consequencial inheritance and ownership root
    ClientMarker const *        ownership_parent() const;
    std::unordered_set< ClientMarker const * > ownership_children() const;

protected:
    virtual bool            log_process_enabled( LogId id ) const override final;
    virtual void            log_process( SrcInfo const & info, LogId id, std::string const & message ) const override final;

private:
    void inherits(){}
    void owns(){}
    
    using StatusPrinterW = void (*)( void * client_ptr, TableDebugView * );
    
    template< class ClientType >
    static void StatusPrinterWImpl( void * client_ptr, TableDebugView * );
    
private:
    //--- set by ClientMarker::mark
    Instance *              inst;
    std::string             _name;
    void *                  _client_ptr;
    std::type_index         _client_type;
    unsigned                _marker_id; ///< Id unique to this ClientMarker's name.
    
    StatusPrinterW          status_printer_w;
    
    //---- relations
    std::unordered_set< ClientMarker const * >  inh_parents;
    ClientMarker const *                        inh_child;
    
    std::unordered_set< ClientMarker const * >  own_children;
    ClientMarker const *                        own_parent;
};

/**
    Static instance used to distribute unique ClientMarker::_marker_id to ClientMarkers.
*/
class ClientMarkerIds
{
public:
    static unsigned UniqueId( std::string name );
    
private:
    static std::unordered_map< std::string, unsigned > * Index;
};

//--------------- StringIO -------------------------
template<>
struct StringIO< ClientMarker >
{
    /*
    std::nullptr_t Read( const char * source, Context const * context ) const
    {
        return nullptr;
    }*/
    
    std::string Write( ClientMarker const & cm, Context const * context ) const
    {
        return cm.root_name_id();
    }
};

template<>
struct StringIO< ClientMarker const * >
{
    ClientMarker const * Read( const char * source, Context const * context ) const
    {
        return nullptr;
    }
    
    std::string Write( ClientMarker const * const & cm, Context const * context ) const
    {
        return cm->root_name_id();
    }
};

}

#include "../Basics/StringIO_defs.hpp"
#include "Instance.hpp"
#include "ClientMarker.inl"
