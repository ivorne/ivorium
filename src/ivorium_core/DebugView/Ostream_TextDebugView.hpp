#pragma once

#include <iostream>

#include "DebugView.hpp"
#include "TextDebugView.hpp"

namespace iv
{

/**
    TODO - maybe put some max-width here and split lines into segments of that width all using the prefix
        - will this work with table output??
*/
class Ostream_TextDebugView : public TextDebugView
{
public:
                    Ostream_TextDebugView( Context * context, std::ostream * out );

    virtual void    print_line( const char * prefix, const char * line, const char * postfix ) override;
    
private:
    std::ostream * _out;
};
    
}
