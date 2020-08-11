namespace iv
{

template< class T, class ... TT >
void Context::WriteItemsToStream( multiline_ostream & mss, T const & first, TT const & ... rest ) const
{
    // first
    mss.multiline_begin();
    mss << StringIO_Write( first, this );
    mss.multiline_end();
    
    // rest
    this->WriteItemsToStream( mss, rest ... );
}

template< class ... TT >
void Context::WriteItemsToStream( multiline_ostream & mss, Endl, TT const & ... rest ) const
{
    // endl
    mss << std::endl;
    
    // rest
    this->WriteItemsToStream( mss, rest ... );
}

template< class ... TT >
void Context::WriteItemsToStream( multiline_ostream & mss, Begin, TT const & ... rest ) const
{
    // begin
    mss.multiline_begin();
    
    // rest
    this->WriteItemsToStream( mss, rest ... );
}

template< class ... TT >
void Context::WriteItemsToStream( multiline_ostream & mss, End, TT const & ... rest ) const
{
    // end
    mss.multiline_end();
    
    // rest
    this->WriteItemsToStream( mss, rest ... );
}

template< class ... Items >
void Context::log( SrcInfo const & info, LogId id, Items const & ... items ) const
{
    if( !this->log_enabled( id ) )
        return;
    
    this->mss.clear();
    this->ss.str( "" );
    this->ss.clear();
    
    this->WriteItemsToStream( this->mss, items ... );
    
    this->log_process( info, id, this->ss.str() );
    
    this->mss.clear();
    this->ss.str( "" );
    this->ss.clear();
}

template< class ... Items >
void Context::warning( SrcInfo const & info, Items const & ... items ) const
{
    this->log( info, ::iv::Defs::Log::Warning, items ... );
}

}
