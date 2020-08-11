#include "DebugView.hpp"

#include "../Instancing/ClientMarker.hpp"

namespace iv
{

DebugView::DebugView( Context * context ) :
    _context( context )
{
}

Context * DebugView::context() const
{
    return this->_context;
}

}
