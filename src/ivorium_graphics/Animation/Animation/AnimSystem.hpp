#pragma once

#include "AnimConnector.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <unordered_set>
#include <vector>

namespace iv
{

class AnimNodeI;
class AnimConnector;

class AnimSystem : public System
{
public:
                                AnimSystem( SystemContainer * sc );

    virtual void                status( TextDebugView * view ) override;
    virtual std::string         debug_name() const override { return "AnimSystem"; }
    
    //
    void                        AnimUpdate( int delta_ms );
    
    // roots
    void                        InsertRoot( AnimNodeI * client );
    void                        RemoveRoot( AnimNodeI * client );
    
    unsigned                    update_id();

private:
    void                        AnimUpdate_Connector( AnimConnector * connector );
    void                        AnimUpdate_Node( AnimNodeI * node );

private:
    struct PrintState
    {
        std::unordered_set< AnimNodeI const * > printed_nodes;
        std::unordered_set< AnimConnector const * > printed_connectors;
    };

    void debug_print_connector( TreeDebugView * tree, AnimConnector * connector, PrintState & state );
    void debug_print_node( TreeDebugView * tree, AnimNodeI * node, PrintState & state );
    
private:
    // index
    std::unordered_set< AnimNodeI * > roots;
    
    // state
    unsigned _update_id;
    
    // cache
    std::vector< AnimConnector * > connectors;
    unsigned nodes_cnt;
};

}
