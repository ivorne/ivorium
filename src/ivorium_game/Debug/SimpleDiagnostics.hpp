#pragma once

#include "SimpleLogger.hpp"

#include <ivorium_graphics/ivorium_graphics.hpp>
#include <ivorium_systems/ivorium_systems.hpp>
#include <ivorium_core/ivorium_core.hpp>

#include <unordered_set>

namespace iv
{

class SimpleDiagnostics : public InputNode, public ConfigStream, private Ostream_TextDebugView, private SimpleLogger
{
public:
ClientMarker cm;
using InputNode::instance;

                        SimpleDiagnostics( Instance * inst );
    virtual             ~SimpleDiagnostics(){}
    
    void                print_systems();
    //void              AddGlobalLog( LogId id );
    
    void                StartupDump();
    
protected:
    // InputNode
    virtual bool        input_trigger_process( InputRoot * root, Input::DeviceKey key ) override;
    
    // ConfigStream
    virtual void        config_stream_changed() override;
    
    // SimpleLogger
    virtual SimpleLogger::Flags ClientLogFlags( ClientMarker const * marker, LogId id ) override;
    virtual SimpleLogger::Flags SystemLogFlags( System const * system, LogId id ) override;
    
private:
    struct LogLine
    {
        std::optional< LogId >          log_id;
        SimpleLogger::Flags             flags;
        bool                            frame;
        
        LogLine() : log_id(), flags( SimpleLogger::Flags::Enabled ), frame( false ){}
    };
    
    struct SimpleLogger_InstanceLog
    {
        std::optional< std::string >    instance_name;
        LogLine                         line;
    };
    
    struct SimpleLogger_ClientLog
    {
        std::optional< std::string >    client_name;
        std::optional< int >            client_id;
        LogLine                         line;
    };
    
    struct SimpleLogger_SystemLog
    {
        std::optional< std::string >    system_name;
        LogLine                         line;
    };
    
    struct SimpleLogger_GeneralLog
    {
        LogLine                         line;
    };
    
private:
    void                                AcceptLogLine( Lex & lex, LogLine & line );
    SimpleLogger::Flags                 LogFlagsCombine( SimpleLogger::Flags a, SimpleLogger::Flags b );
    SimpleLogger::Flags                 LineLogFlags( LogLine const & line, bool frame_enabled, LogId id );
    
    void                                print_welcome();
    bool                                action( unsigned key );
    void                                print_system( System * sys );
    
private:
    // configuration
    bool reading;
    unsigned frame_enabled;
    std::vector< SimpleLogger_InstanceLog >     instanceLog;
    std::vector< SimpleLogger_ClientLog >       clientLog;
    std::vector< SimpleLogger_SystemLog >       systemLog;
    std::vector< SimpleLogger_GeneralLog >      generalLog;
};

}
