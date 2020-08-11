#pragma once

#include "TextDebugView.hpp"
#include <unordered_set>

namespace iv
{

/**
    Adds automatic tree structuring (using box drawing characters) to TextDebugView interface.
    It outputs to another TextDebugView.
*/
class TreeDebugView : public TextDebugView
{
public:
    enum class Style
    {
        BoldFramesWeakLinks,
        WeakFramesWeakLinks
    };
    
    enum class BoxStyle
    {
        Solid,
        Dotted,
        
        Default = Solid
    };
    
public:
                                TreeDebugView( Context * context );
                                
    void                        Push( const char * node_name, BoxStyle box_style = BoxStyle::Default );
    void                        Pop();
    
    void                        Separator();
    
    void                        Write( Style style, TextDebugView * target );
    void                        Clear();
    
protected:
    virtual void                print_line( const char * prefix, const char * line, const char * postfix ) override;
    
private:
    struct Node
    {
        Node * parent;
        std::string name;
        BoxStyle box_style;
        
        std::vector< std::string > lines;
        std::vector< Node * > children;
        std::unordered_set< size_t > separators;
        
        Node( Node * parent, const char * name, BoxStyle box_style ) :
            parent( parent ),
            name( name ),
            box_style( box_style )
        {
        }
        
        ~Node()
        {
            for( Node * n : children )
                delete n;
        }
        
        Node( Node const & ) = delete;
        Node( Node && ) = delete;
        
        Node & operator=( Node const & ) = delete;
        Node & operator=( Node && ) = delete;
    };

    void draw_node_children( Style style, TextDebugView * target, Node * node );

private:
    Node root;
    Node * current;
};

}
