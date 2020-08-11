#include "ClientMarker.hpp"

#include "../DebugView/DebugView.hpp"
#include "../DebugView/TreeDebugView.hpp"
#include "../DebugView/ToText_TableDebugView.hpp"

namespace iv
{

//--------------------- ClientMarkerIds --------------------------------------------
std::unordered_map< std::string, unsigned > * ClientMarkerIds::Index;

unsigned ClientMarkerIds::UniqueId( std::string name )
{
    if( !Index )
        Index = new std::unordered_map< std::string, unsigned >;
    auto & index = *Index;
    
    return index[ name ]++;
}

//--------------------- ClientMarker -----------------------------------------------
ClientMarker::~ClientMarker()
{
    if( this->inst )
        this->inst->client_unregister( this );
}

std::string const & ClientMarker::name() const
{
    return this->_name;
}

unsigned ClientMarker::marker_id() const
{
    return this->_marker_id;
}

std::type_index ClientMarker::client_type() const
{
    return this->_client_type;
}

std::type_index ClientMarker::type() const
{
    return this->_client_type;
}

Instance * ClientMarker::instance() const
{
    return this->inst;
}

ClientMarker const * ClientMarker::inheritance_root() const
{
    if( this->inh_child )
        return this->inh_child->inheritance_root();
    else
        return this;
}

ClientMarker const * ClientMarker::inheritance_child() const
{
    return this->inh_child;
}

std::unordered_set< ClientMarker const * > ClientMarker::inheritance_parents() const
{
    return this->inh_parents;
}

ClientMarker const * ClientMarker::relation_root() const
{
    if( this->inh_child )
        return this->inh_child->relation_root();
    else if( this->own_parent )
        return this->own_parent->relation_root();
    else
        return this;
}

ClientMarker const * ClientMarker::ownership_parent() const
{
    return this->own_parent;
}

std::unordered_set< ClientMarker const * > ClientMarker::ownership_children() const
{
    return this->own_children;
}

void ClientMarker::print_status( TableDebugView * view ) const
{
    if( this->status_printer_w )
        this->status_printer_w( this->_client_ptr, view );
}

void ClientMarker::print_status( TextDebugView * view ) const
{
    ToText_TableDebugView table( view->context() );
    this->print_status( &table );
    
    view->prefix_push( " " );
    view->postfix_push( " " );
    table.Write_AsLines( view, false );
    view->postfix_pop();
    view->prefix_pop();
}

void ClientMarker::print_status_with_inherited( TextDebugView * view, ClientMarker * emphasize ) const
{
    TreeDebugView tree( view->context() );
    this->print_status_with_inherited( &tree, emphasize );
    tree.Write( TreeDebugView::Style::WeakFramesWeakLinks, view );
}

void ClientMarker::print_status_with_inherited( TreeDebugView * view, ClientMarker * emphasize ) const
{
    auto style = TreeDebugView::BoxStyle::Dotted;
    if( this == emphasize )
        style = TreeDebugView::BoxStyle::Solid;
    
    //
    view->Push( this->name_id().c_str(), style );
    
    // print own content
    this->print_status( view );
    
    // print inherited ClientMarkers
    for( ClientMarker const * parent : this->inheritance_parents() )
    {
        parent->print_status_with_inherited( (TreeDebugView*)view, emphasize );
    }
    
    view->Pop();
}

void ClientMarker::print_status_with_related( TextDebugView * view ) const
{
    TreeDebugView tree( view->context() );
    this->print_status_with_related( &tree );
    tree.Write( TreeDebugView::Style::WeakFramesWeakLinks, view );
}

void ClientMarker::print_status_with_related( TreeDebugView * view, TreeDebugView::BoxStyle box_style ) const
{
    view->Push( this->name_id().c_str(), box_style );
    
    {
        // print own content
        this->print_status( view );
        
        // print inherited ClientMarkers
        for( ClientMarker const * parent : this->inheritance_parents() )
            parent->print_status_with_related( (TreeDebugView*)view, TreeDebugView::BoxStyle::Dotted );
        
        // print owned ClientMarkers
        for( ClientMarker const * owned : this->ownership_children() )
            owned->print_status_with_related( (TreeDebugView*)view, TreeDebugView::BoxStyle::Solid );
    }
    
    view->Pop();
}

std::string ClientMarker::name_id() const
{
    return SS() << this->name() << ":" << this->marker_id() << SS::str();
}

std::string ClientMarker::root_name_id() const
{
    return this->inheritance_root()->name_id();
}

bool ClientMarker::log_process_enabled( LogId id ) const
{
    return this->inst->client_log_enabled( this, id );
}

void ClientMarker::log_process( SrcInfo const & info, LogId id, std::string const & message ) const
{
    this->inst->client_log( this, info, id, message );
}

}
