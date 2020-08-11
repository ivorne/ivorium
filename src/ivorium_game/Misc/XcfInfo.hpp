#pragma once

#include <ivorium_graphics/ivorium_graphics.hpp>
#include <ivorium_systems/ivorium_systems.hpp>
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class XcfInfo;

//---------------------------------------------------------------
class XcfInfo_Resource : public SingularResource< XcfInfo >
{
public:
ClientMarker cm;
    XcfInfo_Resource( Instance * inst, ResourcePath const & path ) :
        SingularResource< XcfInfo >( inst, path ),
        cm( inst, this, "XcfInfo_Resource" )
    {
        this->cm.inherits( this->SingularResource< XcfInfo >::cm );
    }
};

//---------------------------------------------------------------
class XcfInfo_Subprovider : public Plain_StreamResourceSubprovider
{
public:
ClientMarker cm;
    XcfInfo_Subprovider( Instance * inst, StreamResourceProvider const * provider ) :
        Plain_StreamResourceSubprovider( inst, provider, "xcf" ),
        cm( inst, this, "XcfInfo_Subprovider" )
    {
        this->cm.inherits( this->Plain_StreamResourceSubprovider::cm );
    }
};

//---------------------------------------------------------------
class XcfInfo : private StreamResource
{
public:
    struct Layer
    {
        float2      size;
        
        float       global_left;
        float       global_right;
        float       global_top;
        float       global_bottom;
        
        float       local_left;
        float       local_right;
        float       local_top;
        float       local_bottom;
        
        Layer() : size(), global_left( 0 ), global_right( 0 ), global_top( 0 ), global_bottom( 0 ), local_left( 0 ), local_right( 0 ), local_top( 0 ), local_bottom( 0 ){}
    };
    
public:
using StreamResource::instance;
ClientMarker cm;
                        XcfInfo( Instance * inst, StreamResourceProvider const * provider, XcfInfo_Subprovider const *, ResourcePath const & path );
    
    float2              size() const;
    Layer const &       layer( char const * name ) const;
    
private:
    void                Parse( std::istream & in );
    
private:
    float2 _size;
    std::unordered_map< std::string, Layer > _layers;
};

}
