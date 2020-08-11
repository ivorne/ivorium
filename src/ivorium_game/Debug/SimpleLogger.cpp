#include "SimpleLogger.hpp"

namespace iv
{

const StringIO< SimpleLogger::Flags >::ValuesType StringIO< SimpleLogger::Flags >::Values = 
{
    // 0
    { SimpleLogger::Flags::Disabled, "Disabled" },
    
    // 1
    { SimpleLogger::Flags::Enabled, "Enabled" },
    { SimpleLogger::Flags::Critical, "Critical" },
    { SimpleLogger::Flags::SrcInfo, "SrcInfo" },
    { SimpleLogger::Flags::InstanceDump, "InstanceDump" },
};

SimpleLogger::SimpleLogger( Instance * inst ) :
    DebugInstanceListener( inst, inst->getSystem< InstanceSystem >() ),
    cm( inst, this, "SimpleLogger" ),
    text_view( &this->cm, &iv::TextOutput ),
    trace_change_counter( 0 )
{
    // prefix used for trace info
    this->text_view.prefix_push( "  " );
}

void SimpleLogger::Print( std::string name, LogId id, std::string entry )
{
    // consider changing trace info
    unsigned counter = LogTrace::GetChangeCounter();
    if( this->trace_change_counter != counter )
    {
        this->trace_change_counter = counter;
        this->text_view.prefix_pop();
        LogTrace::PrintTrace( this->text_view.out() );
        this->text_view.prefix_push( "  " );
    }
    
    //
    for( int i = 0; i < 25 - (int)name.length(); i++ )
        this->text_view.out() << " ";
    
    this->text_view.out() << name << " | " << StringIO_Write( id, &this->cm ) << ": ";
    this->text_view.prefix_push( "                          | " );
    this->text_view.out() << entry << std::endl;
    this->text_view.prefix_pop();
}

void SimpleLogger::ClientLog( ClientMarker const * marker, SrcInfo const & info, LogId id, std::string const & message )
{
    Flags flags = this->ClientLogFlags( marker, id );
    
    if( int( flags ) & int( Flags::Enabled ) )
    {
        this->Print( marker->name_id(), id, message );
    }
    
    if( int( flags ) & int( Flags::SrcInfo ) )
    {
        this->text_view.out() << "    (emitted from " << info.file << ":" << info.line << ")" << std::endl; 
    }
    
    if( int( flags ) & int( Flags::InstanceDump ) )
    {
        TreeDebugView tree( &this->cm );
        tree.Push( marker->instance()->instance_name().c_str() );
        marker->instance()->debug_print_clients( &tree );
        tree.Write( TreeDebugView::Style::BoldFramesWeakLinks, &this->text_view );
    }
    
    if( int( flags ) & int( Flags::Critical ) )
    {
        // break / abort
        #ifndef NDEBUG
        raise( SIGABRT );
        #endif
    }
}

bool SimpleLogger::ClientLogEnabled( ClientMarker const * marker, LogId id )
{
    return int( this->ClientLogFlags( marker, id ) ) & int( Flags::Enabled );
}

void SimpleLogger::SystemLog( System const * system, SrcInfo const & info, LogId id, std::string const & message )
{
    Flags flags = this->SystemLogFlags( system, id );
    
    if( int( flags ) & int( Flags::Enabled ) )
    {
        this->Print( system->debug_name(), id, message );
    }
    
    if( int( flags ) & int( Flags::SrcInfo ) )
    {
        this->text_view.out() << "    (emitted from " << info.file << ":" << info.line << ")" << std::endl; 
    }
    
    if( int( flags ) & int( Flags::Critical ) )
    {
        // break / abort
        #ifndef NDEBUG
        raise( SIGABRT );
        #endif
    }
}

bool SimpleLogger::SystemLogEnabled( System const * system, LogId id )
{
    return int( this->SystemLogFlags( system, id ) ) & int( Flags::Enabled );
}

}
