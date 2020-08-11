#pragma once

#include <iostream>
#include <string>
#include <unordered_set>

namespace iv
{

class TextOutput_streambuf : public std::streambuf
{
public:
    TextOutput_streambuf();
    virtual std::streambuf::int_type overflow( std::streambuf::int_type value ) override;
    
private:
    void flush_as_line();
private:
    std::string buffer;
};

class TextOutput_ostream : public std::ostream
{
public:
    TextOutput_ostream();
    ~TextOutput_ostream();
};

extern TextOutput_ostream TextOutput;

}
