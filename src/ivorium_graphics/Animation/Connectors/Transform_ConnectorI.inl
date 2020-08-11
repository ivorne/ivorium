namespace iv
{

template< class TParent, class TChild >
Transform_ConnectorI< TParent, TChild >::Transform_ConnectorI( Instance * inst ) :
    AnimConnector( inst ),
    cm( inst, this, "Transform_ConnectorI" ),
    parent( nullptr ),
    child( nullptr ),
    _treeChanged( false )
{
    this->cm.inherits( this->AnimConnector::cm );
}

template< class TParent, class TChild >
Transform_ConnectorI< TParent, TChild >::~Transform_ConnectorI()
{
    if( this->parent )
        this->parent->anim_removeChild( this );
    
    if( this->child )
        this->child->anim_unsetParent( this );
}


template< class TParent, class TChild >
bool Transform_ConnectorI< TParent, TChild >::treeChanged_Get()
{
    return this->_treeChanged;
}

template< class TParent, class TChild >
void Transform_ConnectorI< TParent, TChild >::treeChanged_Clear()
{
    this->_treeChanged = false;
}

template< class TParent, class TChild >
void Transform_ConnectorI< TParent, TChild >::anim_eachChild( std::function< void( AnimNodeI * ) > const & f )
{
    if( this->child )
        f( this->child );
}

template< class TParent, class TChild >
void Transform_ConnectorI< TParent, TChild >::anim_childDisconnect( AnimNodeI * node )
{
    if( this->child == node )
    {
        this->child->anim_unsetParent( this );
        this->child = nullptr;
        this->Activate();
        this->_treeChanged = true;
    }
}

template< class TParent, class TChild >
void Transform_ConnectorI< TParent, TChild >::anim_eachParent( std::function< void( AnimNodeI * ) > const & f )
{
    if( this->parent )
        f( this->parent );
}

template< class TParent, class TChild >
void Transform_ConnectorI< TParent, TChild >::anim_parentDisconnect( AnimNodeI * node )
{
    if( this->parent == node )
    {
        this->parent->anim_removeChild( this );
        this->parent = nullptr;
        this->Activate();
        this->_treeChanged = true;
    }
}


template< class TParent, class TChild >
void Transform_ConnectorI< TParent, TChild >::SetParent( AnimNode< TParent > * node )
{
    if( node )
    {
        this->parent = node;
        this->parent->anim_addChild( this );
        this->Activate();
        this->_treeChanged = true;
    }
    else
    {
        this->parent->anim_removeChild( this );
        this->parent = nullptr;
        this->Activate();
        this->_treeChanged = true;
    }
}

template< class TParent, class TChild >
void Transform_ConnectorI< TParent, TChild >::SetChild( AnimNode< TChild > * node )
{
    if( node )
    {
        this->child = node;
        this->child->anim_setParent( this );
        this->Activate();
        this->_treeChanged = true;
    }
    else
    {
        this->child->anim_unsetParent( this );
        this->child = nullptr;
        this->Activate();
        this->_treeChanged = true;
    }
}

}
