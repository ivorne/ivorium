#include "static_warning.hpp"

#include "TextOutput.hpp"

#include <iostream>
#include <csignal>

namespace iv
{

void startup_warning( SrcInfo info, const char * message )
{
    iv::TextOutput << info.file << ":" << info.line << ": startup_warning: " << message << std::endl;
    
    // break / abort
    #ifndef NDEBUG
    //raise( SIGTRAP );
    raise( SIGABRT );
    #endif
}

void runtime_warning( SrcInfo info, const char * message )
{
    iv::TextOutput << info.file << ":" << info.line << ": runtime_warning: " << message << std::endl;
    
    // break / abort
    #ifndef NDEBUG
    //raise( SIGTRAP );
    raise( SIGABRT );
    #endif
}

}
