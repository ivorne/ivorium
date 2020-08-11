#include "LogTrace.hpp"

namespace iv
{

thread_local LogTrace * LogTrace::first;
thread_local unsigned LogTrace::change_counter;

LogTrace::LogTrace()
{
    // note the change
    LogTrace::change_counter++;
    
    // add it to the end of stack
    if( !LogTrace::first )
    {
        LogTrace::first = this;
        this->previous = nullptr;
        this->next = nullptr;
    }
    else
    {
        LogTrace * last = LogTrace::first;
        while( last->next )
            last = last->next;
        last->next = this;
        this->previous = last;
        this->next = nullptr;
    }
}

LogTrace::~LogTrace()
{
    // note the change
    LogTrace::change_counter++;
    
    // remove it from the stack
    if( this->previous )
        this->previous->next = this->next;
    else
        LogTrace::first = this->next;
    
    if( this->next )
        this->next->previous = this->previous;
}

unsigned LogTrace::GetChangeCounter()
{
    return LogTrace::change_counter;
}

void LogTrace::PrintTrace( std::ostream & out )
{
    LogTrace * current = LogTrace::first;
    while( current )
    {
        current->PrintTraceLine( out );
        current = current->next;
    }
}

LambdaLogTrace::LambdaLogTrace( std::function< void( std::ostream & ) > const & f ) :
    _lambda( f )
{
}

void LambdaLogTrace::PrintTraceLine( std::ostream & out )
{
    this->_lambda( out );
}

}
