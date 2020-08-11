#pragma once

#include "Mesh.hpp"

namespace iv
{

class SquareMesh_Subprovider
{
public:
ClientMarker cm;

                    SquareMesh_Subprovider( Instance * inst, VirtualResourceProvider const * );
    void            each_resource( std::function< void( ResourcePath const & ) > const & ) const;
    bool            has_resource( ResourcePath const & ) const;
    
    //
    Instance *      instance() const;
    
private:
    Instance * inst;
};


/**
    It is a 1 x 1 suqare mesh.
*/
class SquareMesh : public Mesh, private GlListener
{
public:
    static const constexpr char *   VirtualPath = "/ivorium/virtual/SquareMesh";
    static bool                     HasResourcePath( Instance * inst, ResourcePath const & path );
    
public:
using Mesh::instance;
ClientMarker cm;

                                    SquareMesh( Instance * inst, VirtualResourceProvider const *, SquareMesh_Subprovider const *, ResourcePath const & path );
    virtual                         ~SquareMesh();
    
private:
    //----------------------------- GlListener ---------------------
    virtual void                    GlEnable() override;
    virtual void                    GlDisable() override;
    virtual void                    GlDrop() override;
};

/**
*/
class SquareMesh_Resource : public SingularResource< Mesh >
{
public:
ClientMarker cm;
    SquareMesh_Resource( Instance * inst ) :
        SingularResource< Mesh >( inst, SquareMesh::VirtualPath ),
        cm( inst, this, "SquareMesh_Resource" )
    {
        this->cm.inherits( this->SingularResource< Mesh >::cm );
    }
};

}
