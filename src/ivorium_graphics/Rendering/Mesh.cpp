#include "Mesh.hpp"

namespace iv
{

Mesh::Mesh( Instance * inst ) :
    inst( inst ),
    _mesh()
{
}

Instance * Mesh::instance() const
{
    return this->inst;
}

GlMesh const * Mesh::gl_mesh() const
{
    return &this->_mesh;
}

GlMesh * Mesh::gl_mesh()
{
    return &this->_mesh;
}

}
