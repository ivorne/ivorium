#pragma once

#include "Basics/LogId.hpp"

namespace iv
{
namespace Defs
{

namespace Log
{
    /**
        All errors and warnings go here.
    */
    extern const LogId Warning;
    
    /**
        Warning concerning values inserted by user.
        Less serious than Warning, but usualy 
    */
    extern const LogId UserWarning;
    
    /**
        Notices that inform about possible performance impacting situations.
        This will be turned off by default, it is meant to be turned on during performance tuning.
    */
    extern const LogId Performance;
    
    /**
    */
    extern const LogId Lex;
    
    /**
    */
    extern const LogId NewFrame;
    
    /**
    */
    extern const LogId ConsistencyChecks;
    
    /**
    */
    extern const LogId ConsistencyChecks_Frame;
}

}
}
