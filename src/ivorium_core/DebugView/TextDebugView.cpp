#include "TextDebugView.hpp"

namespace iv
{

//------------------ TextDebugView ------------------------------------------
TextDebugView::TextDebugView( Context * context ) :
    DebugView( context ),
    prefixes(),
    _prefix(),
    postfixes(),
    _postfix(),
    outstream( this )
{
}

std::ostream & TextDebugView::out()
{
    return this->outstream;
}

void TextDebugView::write_line( const char * line )
{
    std::string prefix = this->prefix();
    std::string postfix = this->postfix();
    this->print_line( prefix.c_str(), line, postfix.c_str() );
}

void TextDebugView::init_line()
{
    this->_prefix = "";
    for( auto & pref : this->prefixes )
        this->_prefix = this->_prefix + pref;
        
    this->_postfix = "";
    for( auto & postf : this->postfixes )
        this->_postfix = postf + this->_postfix;
}

void TextDebugView::prefix_push( const char * pref )
{
    this->prefixes.push_back( pref );
}

void TextDebugView::prefix_push_align()
{
    size_t size = this->outstream.get_line_size();
    size_t active = this->_prefix.size();
    size_t full = 0;
    for( auto & pref : this->prefixes )
        full += pref.size();
    
    size_t inactive = 0;
    if( full > active )
        inactive = full - active;
    
    size_t align = 0;
    if( size > inactive )
        align = size - inactive;
    
    std::string prefix;
    for( size_t i = 0; i < align; i++ )
        prefix += " ";
    
    this->prefix_push( prefix.c_str() );
}

void TextDebugView::prefix_pop()
{
    this->prefixes.pop_back();
}

std::string TextDebugView::prefix()
{
    return this->_prefix;
}

void TextDebugView::postfix_push( const char * postf )
{
    this->postfixes.push_back( postf );
}

void TextDebugView::postfix_pop()
{
    this->postfixes.pop_back();
}

std::string TextDebugView::postfix()
{
    return this->_postfix;
}

void TextDebugView::endline()
{
    this->outstream.endline();
}

//---TextDebugView::tdc_streambuf -- 
TextDebugView::tdc_streambuf::tdc_streambuf( TextDebugView * context ) :
    context( context )
{
}

std::streambuf::int_type TextDebugView::tdc_streambuf::overflow( std::streambuf::int_type value )
{
    if( value == traits_type::eof() )
    {
        this->flush_as_line();
    }
    else
    {
        auto c = traits_type::to_char_type( value );
        if( c == '\n' )
        {
            this->flush_as_line();
        }
        else
        {
            if( this->buffer.empty() )
                this->context->init_line();
            this->buffer.push_back( c );
        }
    }
    
    return traits_type::not_eof( value );
}

void TextDebugView::tdc_streambuf::flush_as_line()
{
    this->context->write_line( this->buffer.c_str() );
    this->buffer.clear();
}

void TextDebugView::tdc_streambuf::endline()
{
    if( this->buffer.size() )
        this->flush_as_line();
}

size_t TextDebugView::tdc_streambuf::get_line_size()
{
    return this->buffer.size();
}

//---TextDebugView::tdc_ostream -- 
TextDebugView::tdc_ostream::tdc_ostream( TextDebugView * context ) :
    std::ostream( new tdc_streambuf( context ) )
{
}

TextDebugView::tdc_ostream::~tdc_ostream()
{
    delete this->rdbuf();
}

void TextDebugView::tdc_ostream::endline()
{
    static_cast< TextDebugView::tdc_streambuf * >( this->rdbuf() )->endline();
}

size_t TextDebugView::tdc_ostream::get_line_size()
{
    return static_cast< TextDebugView::tdc_streambuf * >( this->rdbuf() )->get_line_size();
}

}
