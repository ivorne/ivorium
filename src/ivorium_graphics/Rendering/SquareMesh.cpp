#include "SquareMesh.hpp"

namespace iv
{

//-------------- UniversalShader_Subprovider ---------------------------------------------------------------------
SquareMesh_Subprovider::SquareMesh_Subprovider( Instance * inst, VirtualResourceProvider const * ) :
    cm( inst, this, "SquareMesh_Subprovider" ),
    inst( inst )
{
}

Instance * SquareMesh_Subprovider::instance() const
{
    return this->inst;
}

void SquareMesh_Subprovider::each_resource( std::function< void( ResourcePath const & ) > const & f ) const
{
    f( ResourcePath( SquareMesh::VirtualPath ) );
}

bool SquareMesh_Subprovider::has_resource( ResourcePath const & path ) const
{
    return path == SquareMesh::VirtualPath;
}


//-------------- SquareMesh ---------------------------------------------------------------------
SquareMesh::SquareMesh( Instance * inst, VirtualResourceProvider const *, SquareMesh_Subprovider const *, ResourcePath const & path ) :
    Mesh( inst ),
    GlListener( inst ),
    cm( inst, this, "SquareMesh" )
{
    if( this->GlIsEnabled() )
        this->GlEnable();
}

SquareMesh::~SquareMesh()
{
    this->gl_mesh()->DestroyMesh( &this->cm );
}

void SquareMesh::GlEnable()
{
    auto * mesh = this->gl_mesh();
    
    mesh->CreateMesh( &this->cm );
    static const float positions[] = { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f };
    mesh->Load_Positions( &this->cm, positions, 12 );
    static const float texcoords[] = { 0.0f, 1.0f,          0.0f, 0.0f,         1.0f, 1.0f,          1.0f, 0.0f       };
    mesh->Load_TexCoords( &this->cm, texcoords, 8 );
    static const unsigned indices[] = { 0, 1, 2, 3 };
    mesh->Load_Indices( &this->cm, indices, 4, GL_TRIANGLE_STRIP );
}

void SquareMesh::GlDisable()
{
    this->gl_mesh()->DestroyMesh( &this->cm );
}

void SquareMesh::GlDrop()
{
    this->gl_mesh()->DropMesh( &this->cm );
}

}
