#include "SimpleDiagnostics.hpp"

#include "../Defs.hpp"

#include <iostream>

namespace iv
{

SimpleDiagnostics::SimpleDiagnostics( Instance * inst ) :
    InputNode( inst ),
    ConfigStream( inst, "simple_diagnostics" ),
    Ostream_TextDebugView( &this->cm, &iv::TextOutput ),
    SimpleLogger( inst ),
    cm( inst, this, "SimpleDiagnostics" ),
    reading( false ),
    frame_enabled( (unsigned)-1 )
{
    this->cm.inherits( this->InputNode::cm, this->ConfigStream::cm, this->SimpleLogger::cm );
    
    //
    if( !this->stream_exists() )
    {
        this->stream_write( 
            []( std::ostream & out )
            {

out << R"STRING([SimpleLogger]

# examples
#*                          Performance
#system     my_system       Performance
#instance   my_instance     Performance
#client     *               Performance
#client     Slot:*          Performance

# warnings
client * Warning (Critical) (SrcInfo)
system * Warning (Critical) (SrcInfo)
)STRING" << std::endl;

            }
        );
    }
    
    //
    this->config_stream_changed();
    this->print_welcome();
}

void SimpleDiagnostics::StartupDump()
{
    for( auto & [ type, system ] : this->instance()->getSystemContainer()->debug_GetSystems() )
        if( system->log_enabled( ::iv::Defs::Log::StartupDump ) )
            this->print_system( system );
}

SimpleLogger::Flags SimpleDiagnostics::LineLogFlags( LogLine const & line, bool frame_enabled, LogId id )
{
    if( !line.frame || frame_enabled )
        if( !line.log_id.has_value() || line.log_id.value() == id )
            return line.flags;
    return SimpleLogger::Flags::Disabled;
}

SimpleLogger::Flags SimpleDiagnostics::LogFlagsCombine( SimpleLogger::Flags a, SimpleLogger::Flags b )
{
    return SimpleLogger::Flags( int( a ) | int( b ) );
}

SimpleLogger::Flags SimpleDiagnostics::ClientLogFlags( ClientMarker const * marker, LogId id )
{
    // self check
    if( this->reading && id == ::iv::Defs::Log::Warning )
        return SimpleLogger::Flags::Enabled;
    
    //
    unsigned frame_id = this->instance()->frame_id();
    SimpleLogger::Flags flags = SimpleLogger::Flags::Disabled;
    
    // general log
    for( SimpleLogger_GeneralLog const & log : this->generalLog )
    {
        flags = this->LogFlagsCombine( flags, this->LineLogFlags( log.line, this->frame_enabled == frame_id, id ) );
    }
    
    // instance check
    for( SimpleLogger_InstanceLog const & log : this->instanceLog )
    {
        if( !log.instance_name.has_value() || log.instance_name.value() == marker->instance()->instance_name() )
            flags = this->LogFlagsCombine( flags, this->LineLogFlags( log.line, this->frame_enabled == frame_id, id ) );
    }
    
    // client check
    for( SimpleLogger_ClientLog const & log : this->clientLog )
    {
        SimpleLogger::Flags client_flags = this->LineLogFlags( log.line, this->frame_enabled == frame_id, id );
        if( client_flags == SimpleLogger::Flags::Disabled )
            continue;
        
        if( !log.client_name.has_value() )
        {
            flags = this->LogFlagsCombine( flags, client_flags );
        }
        else
        {
            // check marker and all its inheritance children
            ClientMarker const * m = marker;
            while( m )
            {
                if( m->name() == log.client_name.value() )
                    if( !log.client_id.has_value() || log.client_id.value() == m->marker_id() )
                    {
                        flags = this->LogFlagsCombine( flags, client_flags );
                        break;
                    }
                m = m->inheritance_child();
            }
        }
    }
    
    return flags;
}

SimpleLogger::Flags SimpleDiagnostics::SystemLogFlags( System const * system, LogId id )
{
    // self check
    if( this->reading && id == ::iv::Defs::Log::Warning )
        return SimpleLogger::Flags::Enabled;
    
    //
    unsigned frame_id = this->instance()->frame_id();
    SimpleLogger::Flags flags = SimpleLogger::Flags::Disabled;
    
    // general log
    for( SimpleLogger_GeneralLog const & log : this->generalLog )
    {
        flags = this->LogFlagsCombine( flags, this->LineLogFlags( log.line, this->frame_enabled == frame_id, id ) );
    }
    
    // system log
    for( SimpleLogger_SystemLog const & log : this->systemLog )
    {
        if( !log.system_name.has_value() || log.system_name == system->debug_name() )
            flags = this->LogFlagsCombine( flags, this->LineLogFlags( log.line, this->frame_enabled == frame_id, id ) );
    }
    
    return flags;
}

void SimpleDiagnostics::AcceptLogLine( Lex & lex, LogLine & line )
{
    // log id
    if( lex.IsNext( Lex::Name ) )
        line.log_id = LogId( lex.AcceptName().c_str(), &this->cm );
    else
        lex.AcceptOperator( "*" );
    
    // modifiers
    while( lex.IsNextOperator( "(" ) )
    {
        lex.AcceptOperator( "(" );
        std::string flag_str = lex.AcceptName();
        lex.AcceptOperator( ")" );
        
        if( flag_str == "Frame" )
        {
            line.frame = true;
        }
        else
        {
            auto flag = iv::StringIO_Read< SimpleLogger::Flags >( flag_str.c_str(), &this->cm );
            if( flag != SimpleLogger::Flags::Disabled )
                line.flags = SimpleLogger::Flags( int( line.flags ) | int( flag ) );
            else
                lex.LogicFail( "Invalid flag keyword." );
        }
    }
}

void SimpleDiagnostics::config_stream_changed()
{
    this->reading = true;
    
    this->instanceLog.clear();
    this->clientLog.clear();
    this->systemLog.clear();
    
    this->stream_read(
        [&]( std::istream & in )
        {
            Lex lex( this->instance() );
            lex.DisableNewlineSkipping();
            lex.DefineOperator( "[" );
            lex.DefineOperator( "]" );
            lex.DefineKeyword( "log" );
            lex.DefineKeyword( "unpack" );
            lex.DefineOperator( "*" );
            lex.DefineOperator( ":" );
            lex.DefineOperator( "(" );
            lex.DefineOperator( ")" );
            
            lex.Init( in );
            
            Lex_LogTrace _mod( lex );
            
            constexpr int Mode_None = 0;
            constexpr int Mode_SimpleLogger = 1;
            
            int mode = Mode_None;
            
            while( !lex.IsNext( Lex::Eof ) )
            {
                if( lex.IsNext( Lex::Token::Newline ) )
                {
                    lex.Accept( Lex::Token::Newline );
                }
                else if( lex.IsNextOperator( "[" ) )
                {
                    lex.AcceptOperator( "[" );
                    std::string name = lex.AcceptName();
                    lex.AcceptOperator( "]" );
                    
                    if( lex.Failed() )
                        mode = Mode_None;
                    else if( name == "SimpleLogger" )
                        mode = Mode_SimpleLogger;
                    else
                        lex.LogicFail( "Unknown section identifier." );
                    
                    // newline
                    lex.Recover( Lex::Newline );
                    lex.Accept( Lex::Token::Newline );
                }
                else if( mode == Mode_SimpleLogger )
                {
                    if( lex.IsNextOperator( "*" ) )
                    {
                        lex.AcceptOperator( "*" );
                        
                        SimpleLogger_GeneralLog log;
                        
                        // log id and modifiers
                        this->AcceptLogLine( lex, log.line );
                        
                        //
                        if( !lex.Failed() )
                            this->generalLog.push_back( log );
                        
                        // newline
                        lex.Recover( Lex::Newline );
                        lex.Accept( Lex::Token::Newline );
                    }
                    else
                    {
                        std::string name = lex.AcceptName();
                        if( name == "instance" )
                        {
                            SimpleLogger_InstanceLog log;
                            
                            // instance name
                            if( lex.IsNextOperator( "*" ) )
                                lex.AcceptOperator( "*" );
                            else
                                log.instance_name = lex.AcceptName();
                            
                            // log id and modifiers
                            this->AcceptLogLine( lex, log.line );
                            
                            //
                            if( !lex.Failed() )
                                this->instanceLog.push_back( log );
                            
                            // newline
                            lex.Recover( Lex::Newline );
                            lex.Accept( Lex::Token::Newline );
                        }
                        else if( name == "client" )
                        {
                            SimpleLogger_ClientLog log;
                            
                            // client name
                            if( lex.IsNextOperator( "*" ) )
                                lex.AcceptOperator( "*" );
                            else
                                log.client_name = lex.AcceptName();
                            
                            if( log.client_name.has_value() && lex.IsNextOperator( ":" ) )
                            {
                                // separator
                                lex.AcceptOperator( ":" );
                                
                                // client id
                                if( lex.IsNextKeyword( "*" ) )
                                    lex.AcceptKeyword( "*" );
                                else
                                    log.client_id = lex.AcceptInteger();
                            }
                            
                            // log id and modifiers
                            this->AcceptLogLine( lex, log.line );
                            
                            //
                            if( !lex.Failed() )
                                this->clientLog.push_back( log );
                                
                            // newline
                            lex.Recover( Lex::Newline );
                            lex.Accept( Lex::Token::Newline );
                        }
                        else if( name == "system" )
                        {
                            SimpleLogger_SystemLog log;
                            
                            // system name
                            if( lex.IsNextOperator( "*" ) )
                                lex.AcceptOperator( "*" );
                            else
                                log.system_name = lex.AcceptName();
                            
                            // log id and modifiers
                            this->AcceptLogLine( lex, log.line );
                            
                            //
                            if( !lex.Failed() )
                                this->systemLog.push_back( log );
                            
                            // newline
                            lex.Recover( Lex::Newline );
                            lex.Accept( Lex::Token::Newline );
                        }
                        else
                        {
                            lex.AcceptFail();
                        }
                    }
                }
                else
                {
                    lex.AcceptFail();
                }
            }
        }
    );
    
    this->reading = false;
}

bool SimpleDiagnostics::input_trigger_process( InputRoot * root, Input::DeviceKey key )
{
    if( key.first != Input::Key::Character )
    {
        this->cm.log( SRC_INFO, Defs::Log::Input, "Refuse input, not a Character input." );
        return true;
    }
    
    InputQuery iq( this->instance() );
    unsigned character = iq.input_character();
    
    if( this->action( character ) )
    {
        this->cm.log( SRC_INFO, Defs::Log::Input, "Accept input, but let it fall through." );
    }
    else
    {
        this->cm.log( SRC_INFO, Defs::Log::Input, "Refuse input, no action is attached to this character." );
    }
    
    return true;
}

void SimpleDiagnostics::print_system( System * sys )
{
    if( sys )
    {
        this->out() << sys->debug_name() << ":" << std::endl;
        this->prefix_push( "  " );
        sys->status( this );
        this->out() << std::endl;
        this->prefix_pop();
    }
}

void SimpleDiagnostics::print_welcome()
{
    this->out() << "========= SimpleDiagnostics =========" << std::endl;
    this->out() << "  p - Print all systems." << std::endl;
    this->out() << "  e - Print ElementSystem." << std::endl;
    this->out() << "  i - Print InstanceSystem." << std::endl;
    this->out() << "  n - Print AnimSystem." << std::endl;
    this->out() << "  f - Frame log." << std::endl;
    this->out() << "" << std::endl;
    
    this->out() << "  LogId:" << std::endl;
    for( size_t i = 1; i < LogId::ids_count(); i++ )
    {
        LogId id( i );
        this->out() << "      " << id.runtime_value() << " - " << id.persistent_value() << std::endl;
    }
    this->out() << "" << std::endl;
}

bool SimpleDiagnostics::action( unsigned key )
{
    if( key == 'p' )
    {
        this->print_systems();
    }
    else if( key == 'e' )
    {
        auto es = this->instance()->getSystem< ElementSystem >();
        if( es )
        {
            this->print_system( es );
        }
    }
    else if( key == 'i' )
    {
        this->print_system( this->instance()->getSystem< InstanceSystem >() );
    }
    else if( key == 'n' )
    {
        this->print_system( this->instance()->getSystem< AnimSystem >() );
    }
    else if( key == 'b' )
    {
        this->print_system( this->instance()->getSystem< InputBindingSystem >() );
    }
    else if( key == 'f' )
    {
        this->frame_enabled = this->instance()->frame_id() + 1;
    }
    else
    {
        return true;
    }
    
    return false;
}

void SimpleDiagnostics::print_systems()
{
    this->instance()->getSystemContainer()->debug_print( this );
}

}
