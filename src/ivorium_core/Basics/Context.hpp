#pragma once

#include "multiline_ostream.hpp"
#include "../Defs.hpp"
#include <sstream>
#include "StringIO.hpp"

namespace iv
{

/**
    Classes that print log should also take into consideration class LogStack that contains a call stack-like informations about the log.
*/
class Context
{
public:
                            Context();
    virtual                 ~Context(){}
    
    /** 
        This should be used for newlines in logs.
        Newlines in string literals and other items will generate witespace prefix so that the string is alighned with its own starting symbol.
    */
    class Endl{};
    
    /**
        Everything between Begin and End in Context::log items is considered as one item - newlines are appended by whitespace prefix of length equivalend to width of characters on line before Begin.
    */
    class Begin{};
    class End{};
    
    /**
        All the items will be stringified with StringIO_Write and joined together into a (potentialy multiline) log message.
    */
    template< class ... Items >
    void                    log( SrcInfo const & info, LogId id, Items const & ... items ) const;
    
    /**
    */
    bool                    log_enabled( LogId id ) const;
    
    /**
        Calls Context::log with LogId ::ivorium::Defs::Log::Warning.
    */
    template< class ... Items >
    void                    warning( SrcInfo const & info, Items const & ... items ) const;
    
protected:
    /**
        Method log_process will not be called if this returns false.
        This allows us to avoid performance consuming string building.
    */
    virtual bool            log_process_enabled( LogId id ) const = 0;
    
    /**
    */
    virtual void            log_process( SrcInfo const & info, LogId id, std::string const & message ) const = 0;
    
private:
    template< class T, class ... TT >
    void WriteItemsToStream( multiline_ostream & mss, T const & first, TT const & ... rest ) const;
    
    template< class ... TT >
    void WriteItemsToStream( multiline_ostream & mss, Endl, TT const & ... rest ) const;
    template< class ... TT >
    void WriteItemsToStream( multiline_ostream & mss, Begin, TT const & ... rest ) const;
    template< class ... TT >
    void WriteItemsToStream( multiline_ostream & mss, End, TT const & ... rest ) const;
    
    void WriteItemsToStream( multiline_ostream & mss ) const{}
    
private:
    /**
        Stream used for formatting the log messages.
        It is a class member to allow it to reuse memory from previous formatting instead of allocating new memory every time.
    */
    mutable std::stringstream ss;
    mutable multiline_ostream mss;
};

}

#include "Context.inl"
