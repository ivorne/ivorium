#include "XcfInfo.hpp"

namespace iv
{

XcfInfo::XcfInfo( Instance * inst, StreamResourceProvider const * provider, XcfInfo_Subprovider const *, ResourcePath const & path ) :
    StreamResource( inst, provider, path ),
    cm( inst, this, "XcfInfo" )
{
    this->cm.inherits( this->StreamResource::cm );
    
    this->with_resource_stream( [&]( std::istream & in )
    {
        this->Parse( in );
    });
}

float2 XcfInfo::size() const
{
    return this->_size;
}

XcfInfo::Layer const & XcfInfo::layer( char const * name ) const
{
    static XcfInfo::Layer empty;
    auto it = this->_layers.find( name );
    if( it == this->_layers.end() )
    {
        this->cm.warning( SRC_INFO, "Layer '", "' not found in XcfInfo resource ", this->resource_path() );
        return empty;
    }
    
    return it->second;
}

void XcfInfo::Parse( std::istream & in )
{
    Lex lex( this->instance() );
    lex.DefineKeyword( "width" );
    lex.DefineKeyword( "height" );
    lex.DefineKeyword( "layer" );
    lex.DisableNewlineSkipping();
    
    lex.Init( in );
    Lex_LogTrace _mod( lex );
    
    while( !lex.Failed() && !lex.IsNext( Lex::Token::Eof ) )
    {
        if( lex.IsNextKeyword( "width" ) )
        {
            lex.AcceptKeyword( "width" );
            int width = lex.AcceptDouble();
            
            if( !lex.Failed() )
                this->_size.x = float( width );
        }
        else if( lex.IsNextKeyword( "height" ) )
        {
            lex.AcceptKeyword( "height" );
            int height = lex.AcceptDouble();
            
            if( !lex.Failed() )
                this->_size.y = float( height );
        }
        else if( lex.IsNextKeyword( "layer" ) )
        {
            lex.AcceptKeyword( "layer" );
            
            Layer layer;
            
            int global_order = lex.AcceptDouble();
            (void)global_order;
            std::string name = lex.AcceptString();
            
            layer.size.x = float( lex.AcceptDouble() );
            layer.size.y = float( lex.AcceptDouble() );
            
            layer.global_left = float( lex.AcceptDouble() );
            layer.global_right = float( lex.AcceptDouble() );
            layer.global_top = float( lex.AcceptDouble() );
            layer.global_bottom = float( lex.AcceptDouble() );
            
            layer.local_left = float( lex.AcceptDouble() );
            layer.local_right = float( lex.AcceptDouble() );
            layer.local_top = float( lex.AcceptDouble() );
            layer.local_bottom = float( lex.AcceptDouble() );
            
            if( !lex.Failed() )
                this->_layers[ name ] = layer;
        }
        else
        {
            lex.Accept( Lex::Token::Newline );
        }
        
        lex.Recover( Lex::Token::Newline );
        lex.Accept( Lex::Token::Newline );
    }
}

}
