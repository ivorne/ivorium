#pragma once

#include <functional>
#include <iostream>

namespace iv
{

/**
    Can be instantiated on stack to give contextual information to log.
    Used for example during resource parsing to tell user that given log was printed on given line of input file during resource loading.
    Should work a bit like some kind of stack trace.
    This uses thread_local storage for static data so it can be used in multiple threads independently.
    This must be destroyed on the same thread it was constructed (it can be modified to allow that, but this behavior poses no risks for stack allocated LogTrace instances).
*/
class LogTrace
{
public:
                        LogTrace();
                        ~LogTrace();
    
                        LogTrace( LogTrace const & ) = delete;
    LogTrace &          operator=( LogTrace const & ) = delete;
    
    /**
        Prints trace messages for all LogTrace instances that are currently instantiated on current thread.
        Trace messages are printed in order in which they were instantiated.
    */
    static void         PrintTrace( std::ostream & );
    
    /**
        This allows user to only print stack when it changes (eg. we have multiple log lines but want to print only one trace info for them).
        When this is the same as last time, no need to print it again.
    */
    static unsigned     GetChangeCounter();
    
protected:
    /**
        Print current trace info.
        It is typically one line.
        Should end with a newline character.
    */
    virtual void        PrintTraceLine( std::ostream & ) = 0;
    
private:
    LogTrace * previous;
    LogTrace * next;
    static thread_local LogTrace *  first;
    static thread_local unsigned    change_counter;
};


class LambdaLogTrace : public LogTrace
{
public:
                        LambdaLogTrace( std::function< void( std::ostream & ) > const & f );
    
protected:
    virtual void        PrintTraceLine( std::ostream & ) override;
    
private:
    std::function< void( std::ostream & ) > _lambda;
};

}
