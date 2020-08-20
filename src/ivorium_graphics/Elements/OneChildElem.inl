namespace iv
{

template< class ChildT >
OneChildElem< ChildT >::OneChildElem( Instance * inst ) :
    Elem( inst ),
    cm( inst, this, "OneChildElem" ),
    child( &this->cm, nullptr ),
    heap( inst, &this->cm )
{
    this->cm.inherits( this->Elem::cm );
}

template< class ChildT >
OneChildElem< ChildT >::~OneChildElem()
{
    if( this->child.Get() )
    {
        // elem
        this->child.Get()->elem()->elem_setParent( nullptr );
        
        // input
        this->child.Get()->elem()->input_treeRefresh();
        this->child.Get()->elem()->input_deactivate();
        this->child.Get()->elem()->input_setParent( nullptr );
    }
}

template< class ChildT >
template< class T, class ... CArgs >
T * OneChildElem< ChildT >::createChild( CArgs const & ... cargs )
{
    static_assert( std::is_base_of< ChildT, T >::value, "Child must inherit ChildT." );
    auto t = this->heap.template createClient< T >( cargs ... );
    this->setChild( t );
    return t;
}

template< class ChildT >
template< class T, class ... CArgs >
T * OneChildElem< ChildT >::createChild_Instance( char const * name, CArgs const & ... cargs )
{
    static_assert( std::is_base_of< ChildT, T >::value, "Child must inherit ChildT." );
    auto t = this->heap.template createInstance< T >( name, cargs ... );
    this->setChild( t );
    return t;
}

template< class ChildT >
void OneChildElem< ChildT >::setChild( ChildT * child )
{
    if( this->child.Get() )
        this->removeChild();
    
    if( !child )
    {
        this->removeChild();
        return;
    }
    
    // pointer
    this->child.Set( child );
    
    // input
    child->elem()->input_setParent( this );
    this->input_treeRefresh();
    
    // elem
    child->elem()->elem_setParent( this );
}

template< class ChildT >
void OneChildElem< ChildT >::removeChild()
{
    if( !this->child.Get() )
        return;
    
    // input
    this->child.Get()->elem()->input_deactivate();
    this->child.Get()->elem()->input_setParent( nullptr );
    this->input_treeRefresh();
    
    // elem
    this->child.Get()->elem()->elem_setParent( nullptr );
    
    // pointer
    this->child.Set( nullptr );
}

template< class ChildT >
void OneChildElem< ChildT >::elem_eachChild( std::function< void( Elem * ) > const & f )
{
    if( this->child.Get() )
        f( this->child.Get()->elem() );
}

template< class ChildT >
void OneChildElem< ChildT >::elem_childDisconnect( Elem * node )
{
    if( this->child.Get()->elem() == node )
        this->removeChild();
}

template< class ChildT >
void OneChildElem< ChildT >::input_eachChild( std::function< void( InputNode * ) > const & f )
{
    this->Elem::input_eachChild( f );
    
    if( this->child.Get() )
        f( this->child.Get()->elem() );
}

template< class ChildT >
void OneChildElem< ChildT >::input_childDisconnect( InputNode * child )
{
    this->Elem::input_childDisconnect( child );
    
    if( this->child.Get()->elem() == child )
        this->removeChild();
}

}
