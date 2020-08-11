#include "ElementRenderer.hpp"

#include "Elem.hpp"
#include "../Defs.hpp"

namespace iv
{

//-------------- ElementRenderer ----------------------------------
ElementRenderer::ElementRenderer( Instance * inst ) :
    cm( inst, this, "ElementRenderer" ),
    inst( inst ),
    _first_passes( 0 ),
    _second_passes( 0 )
{
}

Instance *  ElementRenderer::instance() const
{
    return this->inst;
}

void ElementRenderer::QueueSecondPass( Elem * node )
{
    this->second_pass.push_back( node );
}

void ElementRenderer::DequeueSecondPass( Elem * node )
{
    // This relies on the fact that items are popped in the opposite order than they were inserted.
    // Elements must ensure that this condition is satisfied.
    if( this->second_pass.size() && this->second_pass.back() == node )
    {
        this->cm.log( SRC_INFO, Defs::Log::ElementSecondPass, "Dequeue ", node->cm, "." );
        this->second_pass.pop_back();
    }
}

void ElementRenderer::RunSecondPasses()
{
    while( this->second_pass.size() )
    {
        Elem * node = this->second_pass.back();
        this->cm.log( SRC_INFO, Defs::Log::ElementSecondPass, "Start second pass on ", node->cm, "." );
        node->second_pass( this );
    }
}

void ElementRenderer::init_frame()
{   
    this->_first_passes = 0;
    this->_second_passes = 0;
    
    this->FrameStart();
    
    if( !this->second_pass.empty() )
    {
        this->cm.warning( SRC_INFO, "Second pass queue was not cleared in previous frame. Problem in Elements scene tree." );
        this->second_pass.clear();
    }
}

void ElementRenderer::end_frame()
{
    // print stats
    this->cm.log( SRC_INFO, Defs::Log::ElementRefreshSummary, this->_first_passes, " first pass refreshes, ", this->_second_passes, " second pass refreshes" );
}

void ElementRenderer::Notify_FirstPass_Refresh( Elem * elem )
{
    this->_first_passes++;
}

void ElementRenderer::Notify_SecondPass_Refresh( Elem * elem )
{
    this->_second_passes++;
}

}
