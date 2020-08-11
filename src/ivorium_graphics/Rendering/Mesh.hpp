#pragma once

#include "GlSystem.hpp"
#include "../OpenGL/GlMesh.hpp"

#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

/**
*/
class Mesh
{
public:
                                    Mesh( Instance * inst );
    Instance *                      instance() const;
    
    GlMesh const *                  gl_mesh() const;
    
protected:
    GlMesh *                        gl_mesh();
    
private:
    Instance * inst;
    GlMesh _mesh;
};

class Mesh_Resource : public SingularResource< Mesh >
{
public:
ClientMarker cm;

    Mesh_Resource( Instance * inst, ResourcePath const & path ) :
        SingularResource< Mesh >( inst, path ),
        cm( inst, this, "Mesh_Resource" )
    {
        this->cm.inherits( this->SingularResource< Mesh >::cm );
    }
};

}
