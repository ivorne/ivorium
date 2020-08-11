#pragma once

#include "DebugView.hpp"
#include <streambuf>
#include <iostream>

namespace iv
{
    
//----------------------------------------------------------------------------------------------------------------------
//------------- TextDebugView ---------------------------------------------------------------------------------------
/**
*/
class TextDebugView : public DebugView
{
public:
                                TextDebugView( Context * context );
                                
    std::ostream &              out();
    
    void                        prefix_push( const char * pref );
    void                        prefix_push_align();                ///< Pushes prefix that consists of as many spaces as there are characters in current line (minus length of inactive prefixes because they will be applied to next lines but are not applied to this line).
    void                        prefix_pop();
    
    void                        postfix_push( const char * postf );
    void                        postfix_pop();
    
protected:
    virtual void                print_line( const char * prefix, const char * line, const char * postfix ) = 0;
    void                        endline();  ///< What is currently in buffer (TextDebugView::out()) will be written as line. This can be used if the context to which we print changes so we want to print rest of the buffer to previous context even if it is not whole line.
    
private:
    class tdc_streambuf : public std::streambuf
    {
    public:
        tdc_streambuf( TextDebugView * context );
        virtual std::streambuf::int_type overflow( std::streambuf::int_type value ) override;
        void endline();
        size_t get_line_size();
    private:
        void flush_as_line();
    private:
        std::string buffer;
        TextDebugView * context;
    };
    
    class tdc_ostream : public std::ostream
    {
    public:
        tdc_ostream( TextDebugView * context );
        ~tdc_ostream();
        
        void endline();
        size_t get_line_size();
    };
    
    friend class tdc_streambuf;

    void                        write_line( const char * line );    ///< called from tdc_streambuf when line was finished and needs to be written
    void                        init_line();                        ///< called from tdc_streambuf just before first character is written to new line
    std::string                 prefix();
    std::string                 postfix();
    
private:
    std::vector< std::string >  prefixes;
    std::string                 _prefix;
    std::vector< std::string >  postfixes;
    std::string                 _postfix;
    tdc_ostream                 outstream;
};

}
