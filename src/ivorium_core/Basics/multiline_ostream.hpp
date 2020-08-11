#pragma once

#include <iostream>
#include <vector>

namespace iv
{

class multiline_ostream_streambuf : public std::streambuf
{
public:
    multiline_ostream_streambuf( std::ostream * out );
    virtual std::streambuf::int_type overflow( std::streambuf::int_type value ) override;
    
    void multiline_begin();
    void multiline_end();
    void clear();
    
private:
    std::ostream * out;
    size_t pos;
    std::vector< size_t > lengths;
};

class multiline_ostream : public std::ostream
{
public:
    multiline_ostream( std::ostream * out );
    ~multiline_ostream();
    
    void multiline_begin();
    void multiline_end();
    void clear();
};

}
