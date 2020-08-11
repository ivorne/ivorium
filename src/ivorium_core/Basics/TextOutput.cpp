#include "TextOutput.hpp"
#include <iostream>
#include <ivorium_config.hpp>

#if IV_ENABLE_ANDROID_JNI
    #include <android/log.h>
#endif

namespace iv
{

TextOutput_ostream TextOutput;

TextOutput_streambuf::TextOutput_streambuf()
{
}

std::streambuf::int_type TextOutput_streambuf::overflow( std::streambuf::int_type value )
{
    if( value == traits_type::eof() )
    {
        this->flush_as_line();
    }
    else
    {
        auto c = traits_type::to_char_type( value );
        if( c == '\n' )
            this->flush_as_line();
        else
            this->buffer.push_back( c );
    }
    
    return traits_type::not_eof( value );
}

void TextOutput_streambuf::flush_as_line()
{
    // print line
    #if IV_TEXTOUT_COUT
        std::cout << this->buffer << std::endl;
    #endif
    
    #if IV_ENABLE_ANDROID_JNI
        __android_log_write( ANDROID_LOG_INFO, "ivorium", this->buffer.c_str() );
    #endif
    
    //for( auto listener : this->listeners )
    //  listener->AddLine( this->buffer );
    
    // clear buffer for next line
    this->buffer.clear();
}

TextOutput_ostream::TextOutput_ostream() :
    std::ostream( new TextOutput_streambuf() )
{
}

TextOutput_ostream::~TextOutput_ostream()
{
    delete this->rdbuf();
}

}
