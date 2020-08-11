#include "multiline_ostream.hpp"
#include "utils.hpp"

namespace iv
{

//-------------- multiline_ostream_streambuf ---------------------
multiline_ostream_streambuf::multiline_ostream_streambuf( std::ostream * out ) :
    out( out ),
    pos( 0 )
{
}

std::streambuf::int_type multiline_ostream_streambuf::overflow( std::streambuf::int_type value )
{
    if( value == traits_type::eof() )
    {
    }
    else
    {
        auto c = traits_type::to_char_type( value );
        
        if( c == '\n' )
        {
            size_t length = 0;
            if( this->lengths.size() )
                length = this->lengths[ this->lengths.size() - 1 ];
            
            this->out->put( '\n' );
            
            for( size_t i = 0; i < length; i++ )
                this->out->put( ' ' );
            
            this->pos = length;
        }
        else
        {
            if( utf8_is_first_byte( c ) )
                this->pos++;
            this->out->put( c );
        }
    }
    
    return traits_type::not_eof( value );
}

void multiline_ostream_streambuf::multiline_begin()
{
    this->lengths.push_back( this->pos );
}

void multiline_ostream_streambuf::multiline_end()
{
    if( this->lengths.size() )
        this->lengths.pop_back();
}

void multiline_ostream_streambuf::clear()
{
    this->pos = 0;
    this->lengths.clear();
}

//-------------- multiline_ostream ---------------------
multiline_ostream::multiline_ostream( std::ostream * out ) :
    std::ostream( new multiline_ostream_streambuf( out ) )
{
}

multiline_ostream::~multiline_ostream()
{
    delete this->rdbuf();
}

void multiline_ostream::multiline_begin()
{
    static_cast< multiline_ostream_streambuf * >( this->rdbuf() )->multiline_begin();
}

void multiline_ostream::multiline_end()
{
    static_cast< multiline_ostream_streambuf * >( this->rdbuf() )->multiline_end();
}

void multiline_ostream::clear()
{
    static_cast< multiline_ostream_streambuf * >( this->rdbuf() )->clear();
}


}
