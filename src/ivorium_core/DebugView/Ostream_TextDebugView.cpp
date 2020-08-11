#include "Ostream_TextDebugView.hpp"

namespace iv
{

Ostream_TextDebugView::Ostream_TextDebugView( Context * context, std::ostream * out ) :
    TextDebugView( context ),
    _out( out )
{
}

void Ostream_TextDebugView::print_line( const char * prefix, const char * line, const char * postfix )
{
    (*this->_out) << prefix << line << postfix << std::endl;
}

}
