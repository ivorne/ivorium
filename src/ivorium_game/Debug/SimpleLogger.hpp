#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    Currently does not log any instances that already existed when this was created.
*/
class SimpleLogger : public DebugInstanceListener
{
public:
    enum class Flags
    {
        Disabled    = 0,
        Enabled     = 1<<0,
        Critical    = 1<<1,
        SrcInfo     = 1<<2,
        InstanceDump= 1<<3,
    };

ClientMarker cm;
                        SimpleLogger( Instance * inst );
    
protected:
    virtual Flags       ClientLogFlags( ClientMarker const * marker, LogId id ) = 0;
    virtual Flags       SystemLogFlags( System const * system, LogId id ) = 0;
    
private:
    virtual void        ClientLog( ClientMarker const * marker, SrcInfo const & info, LogId id, std::string const & message ) override;
    virtual bool        ClientLogEnabled( ClientMarker const * marker, LogId id ) override;
    
    virtual void        SystemLog( System const * system, SrcInfo const & info, LogId id, std::string const & message ) override;
    virtual bool        SystemLogEnabled( System const * system, LogId id ) override;
    
    void                Print( std::string name, LogId id, std::string entry );
    
private:
    Ostream_TextDebugView text_view;
    unsigned trace_change_counter;
};


template<>
struct StringIO< SimpleLogger::Flags > : public StringIO_Table< SimpleLogger::Flags >
{
    static const ValuesType Values;
};


}
