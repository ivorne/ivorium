#include "StreamFont.hpp"

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

StreamFont_Subprovider::StreamFont_Subprovider( Instance * inst, StreamResourceProvider const * provider ) :
    Plain_StreamResourceSubprovider( inst, provider, "font" ),
    cm( inst, this, "StreamFont_Subprovider" )
{
    this->cm.inherits( this->Plain_StreamResourceSubprovider::cm );
}

StreamFont::StreamFont( Instance * inst, StreamResourceProvider const * provider, StreamFont_Subprovider const *, ResourcePath const & path ) :
    StreamResource( inst, provider, path ),
    Font( inst ),
    cm( inst, this, "StreamFont" ),
    access( inst )
{
    this->cm.inherits( this->StreamResource::cm, this->Font::cm );
    this->cm.owns( this->access.cm );
    
    this->with_resource_stream(
        [&]( std::istream & in )
        {
            this->Load( in );
        }
    );
}

void StreamFont::ReadAdvance( Lex & lex )
{
    lex.AcceptOperator( "{" );
    
    while( !lex.Failed() && !lex.IsNextOperator( "}" ) )
    {
        lex.AcceptKeyword( "glyph" );
        uint32_t code = lex.AcceptInteger();
        float advance = lex.AcceptDouble();
        float width = lex.AcceptDouble();
        if( !lex.Failed() )
        {
            this->_info.advances[ code ] = advance;
            this->_info.widths[ code ] = width;
        }
    }
    
    lex.RecoverOperator( "}" );
    lex.AcceptOperator( "}" );
}

Font::Variant StreamFont::ReadVariant( Lex & lex )
{
    lex.AcceptOperator( "{" );
    
    Font::Variant variant;
    while( !lex.Failed() && !lex.IsNextOperator( "}" ) )
    {
        if( lex.IsNextKeyword( "texture" ) )
        {
            lex.AcceptKeyword( "texture" );
            std::string filename = lex.AcceptString();
            if( !lex.Failed() )
                variant.texture = this->access.get< Texture >( this->resource_path().get_directory() / filename );
        }
        else
        {
            Glyph glyph;
            
            lex.AcceptKeyword( "glyph" );
            glyph.code = lex.AcceptInteger();
            glyph.pos_x = lex.AcceptInteger();
            glyph.pos_y = lex.AcceptInteger();
            glyph.width = lex.AcceptInteger();
            glyph.height = lex.AcceptInteger();
            glyph.bearing_x = lex.AcceptDouble();
            glyph.bearing_y = lex.AcceptDouble();
            
            variant.glyphs[ glyph.code ] = glyph;
        }
    }
    
    lex.RecoverOperator( "}" );
    lex.AcceptOperator( "}" );
    
    return variant;
}

void StreamFont::Load( std::istream & in )
{
    Lex lex( this->instance() );
    lex.DefineKeyword( "line_height" );
    lex.DefineKeyword( "ascender" );
    lex.DefineKeyword( "descender" );
    lex.DefineKeyword( "max_advance" );
    lex.DefineKeyword( "advance" );
    lex.DefineKeyword( "glyph" );
    lex.DefineKeyword( "variant" );
    lex.DefineKeyword( "msdf" );
    lex.DefineKeyword( "fixed" );
    lex.DefineKeyword( "texture" );
    lex.DefineOperator( "{" );
    lex.DefineOperator( "}" );
    
    Lex_LogTrace _trace( lex );
    
    lex.Init( in );
    
    while( !lex.Failed() && !lex.IsNext( Lex::Eof ) )
    {
        if( lex.IsNextKeyword( "line_height" ) )
        {
            lex.AcceptKeyword( "line_height" );
            this->_info.line_height = lex.AcceptDouble();
        }
        else if( lex.IsNextKeyword( "ascender" ) )
        {
            lex.AcceptKeyword( "ascender" );
            this->_info.ascender = lex.AcceptDouble();
        }
        else if( lex.IsNextKeyword( "descender" ) )
        {
            lex.AcceptKeyword( "descender" );
            this->_info.descender = lex.AcceptDouble();
        }
        else if( lex.IsNextKeyword( "max_advance" ) )
        {
            lex.AcceptKeyword( "max_advance" );
            this->_info.max_advance = lex.AcceptDouble();
        }
        else if( lex.IsNextKeyword( "advance" ) )
        {
            lex.AcceptKeyword( "advance" );
            this->ReadAdvance( lex );
        }
        else
        {
            lex.AcceptKeyword( "variant" );
            if( lex.IsNextKeyword( "msdf" ) )
            {
                lex.AcceptKeyword( "msdf" );
                int size = lex.AcceptInteger();
                Variant variant = this->ReadVariant( lex );
                
                if( !lex.Failed() )
                {
                    variant.size = size;
                    this->_variant_msdf = variant;
                }
            }
            else
            {
                lex.AcceptKeyword( "fixed" );
                int size = lex.AcceptInteger();
                Variant variant = this->ReadVariant( lex );
                
                if( !lex.Failed() )
                {
                    variant.size = size;
                    this->_variant_fixed[ size ] = variant;
                }
            }
        }
    }
}

}
