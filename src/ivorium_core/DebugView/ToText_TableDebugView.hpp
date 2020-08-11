#pragma once

#include "TableDebugView.hpp"
#include "TextDebugView.hpp"

namespace iv
{

/**
    TableDebugView implementation that transforms input given on TableDebugView interface is transformed and sent into given TextDebugView instance.
    Expected usage:
        Instantiate an implementation of TextDebugView,
        instantiate ToText_TableDebugView,
        let game classes print into TableDebugView interface of ToText_TableDebugView,
        flush ToText_TableDebugView, causing it to write gathered data into our TextDebugView which then sends it into its text terminal.
        
    Sample output:
        ┏━━╸data|Visual╺━━┯━━━━━━━━━━━━━━━━━┓
        ┃ -position-      │ -visual_id-     ┃
        ┣━━━━━━━━━━━━━━━━━┿━━━━━━━━━━━━━━━━━┫
        ┃ ( 1, 5 )        │ "trs|reactor"   ┃
        ┠─────────────────┼─────────────────┨
        ┃ ( 3, 3 )        │ "trs|dfgg"      ┃
        ┃  ├╴note: origin │                 ┃
        ┃  └╴xyz: glem    │                 ┃
        ┠─────────────────┼─────────────────┨
        ┃ ( 8, 3 )        │ "trs|reactor"   ┃
        ┠─────────────────┼─────────────────┨
        ┃ ( 2, 8 )        │ "trs|gauss_gun" ┃
        ┠─────────────────┼─────────────────┨
        ┃ ( 1, 4 )        │ "trs|dfsd"      ┃
        ┗━━━━━━━━━━━━━━━━━┷━━━━━━━━━━━━━━━━━┛
*/
class ToText_TableDebugView : public TableDebugView
{
public:
//------------------------------------------------
                /**
                    We keep weak pointer to the TextDebugView so it can be destroyed before us.
                */
                ToText_TableDebugView( Context * context );
                
    /**
        Prints gethered data into target TextDebugView (given in constructor).
    */
    void        Write_AsTables( TextDebugView * out );
    
    /**
    */
    void        Write_AsLines( TextDebugView * out, bool print_table_names );

private:
    std::string write( std::any const & value );
};

}
