#include "AnimSystem.hpp"
#include "AnimNode.hpp"
#include "AnimConnector.hpp"

namespace iv
{

AnimSystem::AnimSystem( SystemContainer * sc ) :
    System( sc ),
    roots(),
    _update_id( 0 )
{
}

unsigned AnimSystem::update_id()
{
    return this->_update_id;
}

void AnimSystem::AnimUpdate_Connector( AnimConnector * connector )
{
    bool ready = true;
    
    // are all parents ready for update?
    connector->anim_eachParent(
        [&]( AnimNodeI * node )
        {
            ready = ready && ( !node->Active() || node->System_UpdateId() == this->update_id() );
        }
    );
    
    //
    if( !ready )
        return;
    
    // prepare update on children nodes
    connector->anim_eachChild(
        [&]( AnimNodeI * node )
        {
            if( node->Active() )
            {
                node->System_UpdateStart();
            }
        }
    );
    
    // update connector
    connector->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_SystemUpdate, "Connector pass down." );
    this->connectors.push_back( connector );
    connector->UpdatePass_Down();
    
    // update children nodes
    connector->anim_eachChild(
        [&]( AnimNodeI * node )
        {
            if( node->Active() )
            {
                this->AnimUpdate_Node( node );
            }
        }
    );
}

void AnimSystem::AnimUpdate_Node( AnimNodeI * node )
{
    node->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_SystemUpdate, "Node update start." );
    this->nodes_cnt += 1;
    
    node->anim_eachChild(
        [&]( AnimConnector * con )
        {
            this->AnimUpdate_Connector( con );
        }
    );
}

void AnimSystem::AnimUpdate( int delta_ms )
{
    // reset cache
    this->connectors.clear();
    this->nodes_cnt = 0;
    
    // next update id
    this->_update_id += 1;
    
    this->log( SRC_INFO, ::iv::Defs::Log::NewFrame, "============================< anim_id ", this->_update_id, " >=================================" );
    
    //
    Anim_float step_distance = Anim_float( delta_ms ) / Anim_float( 1000.0 );
    
    // down pass - change targets and advance towards them
    for( AnimNodeI * root : this->roots )
        if( root->Active() )
        {
            root->System_UpdateStart();
            root->Connector_RequestStep( step_distance );
            this->AnimUpdate_Node( root );
        }
    
    // up pass - refresh distances
    for( auto it = this->connectors.rbegin(); it != this->connectors.rend(); ++it )
    {
        AnimConnector * connector = *it;
        
        // end update on children
        connector->anim_eachChild(
            [&]( AnimNodeI * node )
            {
                if( node->Active() )
                {
                    node->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_SystemUpdate, "Node update end." );
                    node->System_UpdateEnd();
                }
            }
        );
        
        // up pass update on connector
        connector->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_SystemUpdate, "Connector pass up." );
        connector->UpdatePass_Up();
    }
    
    for( AnimNodeI * root : this->roots )
        if( root->Active() )
        {
            root->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_SystemUpdate, "Node update end." );
            root->System_UpdateEnd();
        }
    
    this->log( SRC_INFO, ::iv::Defs::Log::Animation_Summary, "Summary: ", this->connectors.size(), " connectors updated, ", this->nodes_cnt, " nodes updated." );
}

void AnimSystem::InsertRoot( AnimNodeI * client )
{
    this->roots.insert( client );
}

void AnimSystem::RemoveRoot( AnimNodeI * client )
{
    this->roots.erase( client );
}

void AnimSystem::debug_print_connector( TreeDebugView * tree, AnimConnector * connector, PrintState & state )
{
    tree->Push( connector->cm.name_id().c_str() );
    
    if( state.printed_connectors.count( connector ) )
    {
        tree->out() << " ..." << std::endl;
    }
    else
    {
        state.printed_connectors.insert( connector );
        
        TreeDebugView subtree( tree->context() );
        connector->cm.inheritance_root()->print_status_with_inherited( &subtree, &connector->cm );
        subtree.Write( TreeDebugView::Style::WeakFramesWeakLinks, tree );
        
        connector->anim_eachChild(
            [&]( AnimNodeI * node )
            {
                this->debug_print_node( tree, node, state );
            }
        );
    }
    
    tree->Pop();
}

void AnimSystem::debug_print_node( TreeDebugView * tree, AnimNodeI * node, PrintState & state )
{
    tree->Push( ( node->cm.name_id() + " " + node->label() ).c_str() );
    
    if( state.printed_nodes.count( node ) )
    {
        tree->out() << " ..." << std::endl;
    }
    else
    {
        state.printed_nodes.insert( node );
        
        TreeDebugView subtree( tree->context() );
        node->cm.inheritance_root()->print_status_with_inherited( &subtree, &node->cm );
        subtree.Write( TreeDebugView::Style::WeakFramesWeakLinks, tree );
        
        node->anim_eachChild(
            [&]( AnimConnector * connector )
            {
                this->debug_print_connector( tree, connector, state );
            }
        );
    }
    
    tree->Pop();
}

void AnimSystem::status( TextDebugView * view )
{
    TreeDebugView tc( view->context() );
    
    PrintState state;
    
    for( AnimNodeI * node : this->roots )
    {
        this->debug_print_node( &tc, node, state );
    }
    
    tc.Write( TreeDebugView::Style::BoldFramesWeakLinks, view );
}

}
