#pragma once

#include "Font.hpp"
#include <ivorium_systems/ivorium_systems.hpp>
#include <string>
#include <vector>

namespace iv
{

class StreamFont_Subprovider : public Plain_StreamResourceSubprovider
{
public:
ClientMarker cm;
using Plain_StreamResourceSubprovider::instance;
    StreamFont_Subprovider( Instance * inst, StreamResourceProvider const * );
};

class StreamFont : private StreamResource, public Font
{
public:
using StreamResource::instance;
ClientMarker cm;
    
    StreamFont( Instance * inst, StreamResourceProvider const * provider, StreamFont_Subprovider const *, ResourcePath const & path );
    
private:
    void                Load( std::istream & in );
    Font::Variant       ReadVariant( Lex & lex );
    void                ReadAdvance( Lex & lex );
    
private:
    ResourceAccess access;
};


}
