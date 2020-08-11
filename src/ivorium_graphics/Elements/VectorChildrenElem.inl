namespace iv
{

template< class ChildT >
VectorChildrenElem< ChildT >::VectorChildrenElem( Instance * inst ) :
    Elem( inst ),
    cm( inst, this, "VectorChildrenElem" ),
    heap( inst, &this->cm )
{
    this->cm.inherits( this->Elem::cm );
}

template< class ChildT >
VectorChildrenElem< ChildT >::~VectorChildrenElem()
{
    for( ChildT * child : this->children )
    {
        // elem
        child->elem()->elem_setParent( nullptr );
        
        // input
        child->elem()->input_treeRefresh();
        child->elem()->input_deactivate();
        child->elem()->input_setParent( nullptr );
    }
}

template< class ChildT >
template< class T, class ... CArgs >
T * VectorChildrenElem< ChildT >::createChild( CArgs const & ... cargs )
{
    static_assert( std::is_base_of< ChildT, T >::value, "Child must inherit ChildT." );
    auto t = this->heap.template createClient< T >( cargs ... );
    this->pushChild( t );
    return t;
}

template< class ChildT >
template< class T, class ... CArgs >
T * VectorChildrenElem< ChildT >::createChild_Instance( char const * name, CArgs const & ... cargs )
{
    static_assert( std::is_base_of< ChildT, T >::value, "Child must inherit ChildT." );
    auto t = this->heap.template createInstance< T >( name, cargs ... );
    this->pushChild( t );
    return t;
}

template< class ChildT >
void VectorChildrenElem< ChildT >::pushChild( ChildT * child )
{
    // vector
    this->children.push_back( child );
    this->children_dirty = true;
    
    // input
    child->elem()->input_setParent( this );
    this->input_treeRefresh();
    
    // elem
    child->elem()->elem_setParent( this );
}

template< class ChildT >
void VectorChildrenElem< ChildT >::_removeChild( size_t i )
{
    ChildT * child = this->children[ i ];
    
    // input
    child->elem()->input_setParent( nullptr );
    child->elem()->input_deactivate();
    child->elem()->input_treeRefresh();
    
    // elem
    child->elem()->elem_setParent( nullptr );
    
    // vector
    this->children.erase( this->children.begin() + i );
    this->children_dirty = true;
}

template< class ChildT >
void VectorChildrenElem< ChildT >::removeChild( ChildT * child )
{
    this->input_childDisconnect( child->elem() );
}

template< class ChildT >
void VectorChildrenElem< ChildT >::elem_eachChild( std::function< void( Elem * ) > const & f )
{
    for( auto * child : this->children )
        f( child->elem() );
}

template< class ChildT >
void VectorChildrenElem< ChildT >::elem_childDisconnect( Elem * child )
{
    this->input_childDisconnect( child );
}

template< class ChildT >
void VectorChildrenElem< ChildT >::input_eachChild( std::function< void( InputNode * ) > const & f )
{
    this->Elem::input_eachChild( f );
    
    for( auto it = this->children.rbegin(); it != this->children.rend(); ++it )
        f( (*it)->elem() );
}

template< class ChildT >
void VectorChildrenElem< ChildT >::input_childDisconnect( InputNode * child )
{
    this->Elem::input_childDisconnect( child );
    
    for( size_t i = 0; i < this->children.size(); i++ )
        if( this->children[ i ]->elem() == child )
        {
            this->_removeChild( i );
            break;
        }
}

}
