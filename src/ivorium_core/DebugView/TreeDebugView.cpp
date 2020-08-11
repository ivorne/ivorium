#include "TreeDebugView.hpp"
#include "../Basics/utils.hpp"
#include "../Instancing/Instance.hpp"

namespace iv
{

TreeDebugView::TreeDebugView( Context * context ) :
    TextDebugView( context ),
    root( nullptr, "", BoxStyle::Default ),
    current( &this->root )
{
}

void TreeDebugView::Push( const char * node_name, BoxStyle box_style )
{
    this->endline();
    Node * node = new Node( this->current, node_name, box_style );
    this->current->children.push_back( node );
    this->current = node;
}

void TreeDebugView::Pop()
{
    if( !this->current->parent )
    {
        this->context()->warning( SRC_INFO, "Can not pop. This is root." );
        return;
    }
    
    this->endline();
    this->current = this->current->parent;
}

void TreeDebugView::Write( Style style, TextDebugView * target )
{
    //
    this->endline();
    
    // print all
    this->draw_node_children( style, target, &this->root );
}
    
void TreeDebugView::Clear()
{
    //
    this->endline();
    
    // clear
    this->current = &this->root;
    this->root.children.clear();
}

void TreeDebugView::Separator()
{
    if( this->current == &this->root )
    {
        this->context()->warning( SRC_INFO, "There is not an active node. Can not print line." );
        return;
    }
    
    this->endline();
    
    this->current->separators.insert( this->current->lines.size() - 1 );
}

void TreeDebugView::print_line( const char * prefix, const char * line, const char * postfix )
{
    if( this->current == &this->root )
    {
        this->context()->warning( SRC_INFO, "There is not an active node. Can not print line." );
        return;
    }
    
    this->current->lines.push_back( std::string( prefix ) + std::string( line ) + std::string( postfix ) );
}

void TreeDebugView::draw_node_children( Style style, TextDebugView * target, Node * node )
{
    std::vector< const char * > c;
    if( style == Style::BoldFramesWeakLinks )
    {
        c = 
        {
            "│",    // link_line
            "├",    // link_nonlast
            "└",    // link_last
            
            "━",    // hor
            "╸",    // hor_lhalf
            "╺",    // hor_rhalf
            "┯",    // hor_linkout
            
            "┃",    // vert
            "┨",    // vert_linkin
            
            "┏",    // corner_tl
            "┗",    // corner_bl
            "┓",    // corner_tr
            "┛",    // corner_br
            
            "╍",    // hor (dotted)
            "╏",    // vert (dotted)
            "┏",    // corner_tl (dotted)
            "┗",    // corner_bl (dotted)
            "┓",    // corner_tr (dotted)
            "┛",    // corner_br (dotted)
        };
    }
    else 
    {
        if( style != Style::WeakFramesWeakLinks )
            this->context()->warning( SRC_INFO, "Unimplemented style." );
        
        c = 
        {
            "│",    // link_line
            "├",    // link_nonlast
            "└",    // link_last
            
            "─",    // hor
            "╴",    // hor_lhalf
            "╶",    // hor_rhalf
            "┬",    // hor_linkout
            
            "│",    // vert
            "┤",    // vert_linkin
            
            "┌",    // corner_tl
            "└",    // corner_bl
            "┐",    // corner_tr
            "┘",    // corner_br
            
            "╌",    // hor (dotted)
            "╎",    // vert (dotted)
            "╭",    // corner_tl (dotted)
            "╰",    // corner_bl (dotted)
            "╮",    // corner_tr (dotted)
            "╯",    // corner_br (dotted)
        };
    }
    
    
    const int link_line     = 0;
    const int link_nonlast  = 1;
    const int link_last     = 2;
    int hor                 = 3;
    const int hor_lhalf     = 4;
    const int hor_rhalf     = 5;
    const int hor_linkout   = 6;
    int vert                = 7;
    const int vert_linkin   = 8;
    int corner_tl           = 9;
    int corner_bl           = 10;
    int corner_tr           = 11;
    int corner_br           = 12;
    
    
    for( size_t ci = 0; ci < node->children.size(); ci++ )
    {
        //
        Node * ch = node->children[ ci ];
        bool last = ci == node->children.size() - 1;
        
        //
        if( ch->box_style == BoxStyle::Dotted )
        {
            hor = 13;;
            vert = 14;
            
            corner_tl = 15;
            corner_bl = 16;
            corner_tr = 17;
            corner_br = 18;
        }
        else
        {
            hor = 3;
            vert = 7;
            
            corner_tl = 9;
            corner_bl = 10;
            corner_tr = 11;
            corner_br = 12;
        }
        
        //-------- print self -----------
        // compute line width
        size_t inner_width = 0;
        for( std::string const & l : ch->lines )
            inner_width = std::max( inner_width, utf8_size( l ) );
        inner_width = std::max( inner_width, utf8_size( ch->name ) + 6 );
        
        // header
        if( node != &this->root )
            target->out() <<" "<<c[ link_line ];
        if( ch->name.size() )
            target->out() <<c[ corner_tl ]<<c[ hor ]<<c[ hor ]<<c[ hor_lhalf ]<< ch->name <<c[ hor_rhalf ]<<c[ hor ]<<c[ hor ];
        else
            target->out() <<c[ corner_tl ]<<c[ hor ]<<c[ hor ]<<c[ hor ]<<c[ hor ]<<c[ hor ]<<c[ hor ];
        for( size_t i = utf8_size( ch->name ) + 6; i < inner_width ; i++ )
            target->out() << c[ hor ];
        target->out() << c[ corner_tr ] << std::endl;
        
        // first line
        if( node != &this->root )
        {
            if( last )
                target->out() <<" "<<c[ link_last ]<<c[ vert_linkin ];
            else
                target->out() <<" "<<c[ link_nonlast ]<<c[ vert_linkin ];
        }
        else
        {
            target->out() << c[ vert ];
        }
        
        if( ch->lines.size() )
        {
            target->out() << ch->lines[ 0 ];
            for( size_t i = utf8_size( ch->lines[ 0 ] ); i < inner_width; i++ )
                target->out() << " ";
            target->out() << c[ vert ] << std::endl;
        }
        else
        {
            for( size_t i = 0; i < inner_width; i++ )
                target->out() << " ";
            target->out() << c[ vert ] << std::endl;
        }
        
        // one character prefix for next lines
        if( node != &this->root )
        {
            if( last )
                target->prefix_push( "  " );
            else
                target->prefix_push( ( std::string( " " ) + c[ link_line ] ).c_str() );
        }
        
        // separator for first line
        if( ch->separators.count( 0 ) )
        {
            target->out() << c[ vert ];
            for( size_t j = 0; j < inner_width; j++ )
                target->out() << c[ hor ];
            target->out() << c[ vert ] << std::endl;
        }
        
        
        {
            // print rest of lines
            for( size_t i = 1; i < ch->lines.size(); i++ )
            {
                target->out() << c[ vert ] << ch->lines[ i ];
                for( size_t j = utf8_size( ch->lines[ i ] ); j < inner_width; j++ )
                    target->out() << " ";
                target->out() << c[ vert ] << std::endl;
                
                // separator
                if( ch->separators.count( i ) && i < ch->lines.size() - 1 )
                {
                    target->out() << c[ vert ];
                    for( size_t j = 0; j < inner_width; j++ )
                        target->out() << c[ hor ];
                    target->out() << c[ vert ] << std::endl;
                }
            }
            
            // footer
            if( ch->children.size() )
                target->out() <<c[ corner_bl ]<<c[ hor_linkout ];
            else
                target->out() <<c[ corner_bl ]<<c[ hor ];
            for( size_t i = 1; i < inner_width; i++ )
                target->out() << c[ hor ];
            target->out() << c[ corner_br ] << std::endl;
            
            // print children
            this->draw_node_children( style, target, ch );
        }
        
        //
        if( node != &this->root )
            target->prefix_pop();
    }
}

}
