#pragma once

#include <string>

#define SRC_INFO ::iv::SrcInfo( __FILE__, __LINE__, __func__ )

namespace iv
{

struct SrcInfo
{
    std::string     file;
    int             line;
    std::string     func;
    
    SrcInfo( const char * file, int line, const char * func ) : file( file ), line( line ), func( func ){}
};

}
