#include "StreamResourceSubprovider.hpp"

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

//=========== StreamResourceSubprovider ==============================
StreamResourceSubprovider::StreamResourceSubprovider( Instance * inst ) :
    cm( inst, this, "StreamResourceSubprovider" ),
    lex( inst )
{
    this->cm.owns( this->lex.cm );
}

Instance * StreamResourceSubprovider::instance() const
{
    return this->lex.instance();
}

void StreamResourceSubprovider::Parse_EachResource( std::istream & metadata_file )
{
    this->lex.Init( metadata_file );
    
    Lex_LogTrace _trace( this->lex.lex() );
    
    lex.Accept( JsonLex::TableBegin );
    
    // filenames
    while( !this->lex.Failed() )
    {
        if( !this->lex.IsNext( JsonLex::TableKey ) )
            break;
        
        std::string filename = this->lex.AcceptTableKey();
        
        // parameters
        this->lex.Accept( JsonLex::TableBegin );
        
        // class
        std::string cl_key = this->lex.AcceptTableKey();
        std::string cl_val = this->lex.AcceptString();
        
        if( cl_key != "class" )
            this->lex.LogicFail( "Parameter 'class' must be the first specified parameter." );
        
        this->Resource( cl_val, ResourcePath( filename ) );
        
        lex.Accept( JsonLex::TableEnd );
    }
    
    this->lex.Accept( JsonLex::TableEnd );
}

void StreamResourceSubprovider::Parse_EachParameter( std::function< void( std::string const & param, std::string const & val ) > const & f )
{
    while( !this->lex.Failed() )
    {
        if( !this->lex.IsNext( JsonLex::TableKey ) )
            break;
        
        std::string param = this->lex.AcceptTableKey();
        std::string value = this->lex.AcceptString();
        
        if( f && !this->lex.Failed() )
            f( param, value );
    }
}

//========================== Plain_StreamResourceSubprovider ===================================
Plain_StreamResourceSubprovider::Plain_StreamResourceSubprovider( Instance * inst, StreamResourceProvider const * provider, char const * metadata_class ) :
    StreamResourceSubprovider( inst ),
    cm( inst, this, "Plain_StreamResourceSubprovider" ),
    metadata_class( metadata_class )
{
    this->cm.inherits( this->StreamResourceSubprovider::cm );
    
    provider->with_metadata_stream(
        [&]( std::istream & metadata_file )
        {
            this->Parse_EachResource( metadata_file );
        }
    );
    this->metadata_class = nullptr;
}

void Plain_StreamResourceSubprovider::Resource( std::string const & resource_class, ResourcePath path )
{
    if( resource_class == this->metadata_class )
        this->resources.insert( path );
    
    this->Parse_EachParameter();
}

void Plain_StreamResourceSubprovider::each_resource( std::function< void( ResourcePath const & ) > const & f ) const
{
    for( auto resource : this->resources )
        f( resource );
}

bool Plain_StreamResourceSubprovider::has_resource( ResourcePath const & path ) const
{
    return this->resources.count( path );
}

}
