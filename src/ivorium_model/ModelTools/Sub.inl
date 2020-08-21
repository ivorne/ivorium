namespace iv
{

//-----------------------------------------------------------
/**
*/
template< class T >
class Sub_StructorFunctionsT : public Sub_StructorFunctions
{
public:
    static void * T_copy_new( void * src )
    {
        T * t_src = reinterpret_cast< T * >( src );
        T * t_self = new T( *t_src );
        return reinterpret_cast< T * >( t_self );
    }

    static void * T_copy_inplace( void * place, void * src )
    {
        T * t_src = reinterpret_cast< T * >( src );
        T * t_self = new (place) T( *t_src );
        return reinterpret_cast< T * >( t_self );
    }

    static void T_destroy_inplace( void * self )
    {
        T * t_self = reinterpret_cast< T * >( self );
        t_self->~T();
    }
    
    static void T_destroy_delete( void * self )
    {
        T * t_self = reinterpret_cast< T * >( self );
        delete t_self;
    }
    
    static void T_assign_copy( void * self, void * src )
    {
        T * t_self = reinterpret_cast< T * >( self );
        T * t_src = reinterpret_cast< T * >( src );
        (*t_self) = (*t_src);
    }
    
    static void T_assign_default( void * self )
    {
        T * t_self = reinterpret_cast< T * >( self );
        (*t_self) = T();
    }
        
    //static void * T_move_new( void * src )
    //{
    //    T * t_src = reinterpret_cast< T * >( src );
    //    T * t_self = new T( std::move( *t_src ) );
    //    return reinterpret_cast< T * >( t_self );
    //}
    //
    //static void * T_move_inplace( void * place, void * src )
    //{
    //    T * t_src = reinterpret_cast< T * >( src );
    //    T * t_self = new (place) T( std::move( *t_src ) );
    //    return reinterpret_cast< T * >( t_self );
    //}
    //
    //static void T_assign_move( void * self, void * src )
    //{
    //    (*t_self) = std::move( (*t_src) );
    //}
    
    Sub_StructorFunctionsT()
    {
        this->sizeof_v = sizeof( T );
        this->alignof_v = alignof( T );
        this->copy_new = &T_copy_new;
        this->copy_inplace = &T_copy_inplace;
        this->destroy_inplace = &T_destroy_inplace;
        this->destroy_delete = &T_destroy_delete;
        this->assign_copy = &T_assign_copy;
        this->assign_default = &T_assign_default;
    }

    static Sub_StructorFunctionsT< T > Def;
};

template< class T >
Sub_StructorFunctionsT< T > Sub_StructorFunctionsT< T >::Def;

//---------------- SubId -------------------------------------------
template< class Type, class SubType >
SubId< Type, SubType >::SubId() :
    _idx( Type::SubDirectoryInst.next++ )
{
}

template< class Type, class SubType > 
size_t SubId< Type, SubType >::idx() const
{
    return this->_idx;
}

template< class Type, class SubType > 
size_t SubId< Type, SubType >::idx_count() const
{
    return Type::SubDirectoryInst.next;
}

//-------------- Sub -----------------------------------------------
template< class T >
static void l_dynamic_deleter( void * ptr )
{
    delete reinterpret_cast< T * >( ptr );
}

template< class T >
static void l_static_deleter( void * ptr )
{
    T * t = reinterpret_cast< T * >( ptr );
    t->~T();
}

template< class SubType, size_t LocalItems, size_t LocalStorage >
Sub< SubType, LocalItems, LocalStorage >::Sub() :
    dir_local(),
    mem_local(),
    dir_dynamic(),
    static_mem_used( 0 )
{
}

template< class SubType, size_t LocalItems, size_t LocalStorage >
Sub< SubType, LocalItems, LocalStorage >::~Sub()
{
    // deinitialize everything
    for( S & s : iv::reversed( this->dir_dynamic ) )
        if( s.inst )
        {
            if( !s.dynamic )
                s.functions->destroy_inplace( s.inst );
            else
                s.functions->destroy_delete( s.inst );
        }
    for( S & s : iv::reversed( this->dir_local ) )
        if( s.inst )
        {
            if( !s.dynamic )
                s.functions->destroy_inplace( s.inst );
            else
                s.functions->destroy_delete( s.inst );
        }
}

template< class SubType, size_t LocalItems, size_t LocalStorage >
void Sub< SubType, LocalItems, LocalStorage >::clear()
{
    // deinitialize and clear everything
    for( S & s : iv::reversed( this->dir_dynamic ) )
        if( s.inst )
        {
            if( !s.dynamic )
                s.functions->destroy_inplace( s.inst );
            else
                s.functions->destroy_delete( s.inst );
            s.inst = nullptr;
            s.functions = nullptr;
            s.dynamic = false;
        }
    
    for( S & s : iv::reversed( this->dir_local ) )
        if( s.inst )
        {
            if( !s.dynamic )
                s.functions->destroy_inplace( s.inst );
            else
                s.functions->destroy_delete( s.inst );
            s.inst = nullptr;
            s.functions = nullptr;
            s.dynamic = false;
        }
    
    // reset static memory
    this->static_mem_used = 0;
}

template< class Type, size_t LocalItems, size_t LocalStorage >
typename Sub< Type, LocalItems, LocalStorage >::S * Sub< Type, LocalItems, LocalStorage >::getS( size_t idx )
{
    bool dir_dynamic = idx > LocalItems;
    if( !dir_dynamic )
    {
        return &this->dir_local[ idx ];
    }
    else
    {
        size_t dyn_idx = Type::SubDirectoryInst.next - LocalItems;
        this->dir_dynamic.resize( dyn_idx );
        return &this->dir_dynamic[ dyn_idx ];
    }
}

template< class Type, size_t LocalItems, size_t LocalStorage >
template< class SubType >
SubType & Sub< Type, LocalItems, LocalStorage >::operator[]( SubId< Type, SubType > const & id )
{
    // locate pointer
    S * s = this->getS( id.idx() );
    
    // initialize value
    if( !s->inst )
    {
        // try to align it into static storage
        void * ptr = mem_local.data() + this->static_mem_used;
        size_t space = LocalStorage - this->static_mem_used;
        void * aligned = std::align( alignof( SubType ), sizeof( SubType ), ptr, space );
        
        // allocate it
        if( aligned )
        {// static
            this->static_mem_used = LocalStorage - space;
            s->inst = reinterpret_cast< void * >( new ( aligned ) SubType );
            s->functions = &Sub_StructorFunctionsT< SubType >::Def;
            s->dynamic = false;
        }
        else
        {// dynamic
            s->inst = reinterpret_cast< void * >( new SubType );
            s->functions = &Sub_StructorFunctionsT< SubType >::Def;
            s->dynamic = true;
        }
    }
    
    // return value
    return *reinterpret_cast< SubType * >( s->inst );
}

template< class Type, size_t LocalItems, size_t LocalStorage >
void Sub< Type, LocalItems, LocalStorage >::copy( size_t idx, S const & s_src )
{
    // locate pointer
    S * s = this->getS( idx );
    
    // initialize value
    if( s_src.inst )
    {
        if( !s->inst )
        {
            // try to align it into static storage
            void * ptr = mem_local.data() + this->static_mem_used;
            size_t space = LocalStorage - this->static_mem_used;
            void * aligned = std::align( s_src.functions->alignof_v, s_src.functions->sizeof_v, ptr, space );
            
            // allocate it
            if( aligned )
            {// static
                this->static_mem_used = LocalStorage - space;
                s->inst = s_src.functions->copy_inplace( aligned, s_src.inst );
                s->functions = s_src.functions;
                s->dynamic = false;
            }
            else
            {// dynamic
                s->inst = s_src.functions->copy_new( s_src.inst );
                s->functions = s_src.functions;
                s->dynamic = true;
            }
        }
        else
        {
            s_src.functions->assign_copy( s->inst, s_src.inst );
        }
    }
    else
    {
        if( s->inst )
        {
            // If it exists here but not in source, it will not be deinitialized, but only put to default state - so that we don't mess up the static allocator.
            s->functions->assign_default( s->inst );
        }
    }
}

template< class Type, size_t LocalItems, size_t LocalStorage >
Sub< Type, LocalItems, LocalStorage >::Sub( Sub const & src )
{
    size_t idx = 0;
    
    for( S const & s : src.dir_local )
        if( s.inst )
        {
            this->copy( idx, s );
            idx += 1;
        }
            
    for( S const & s : src.dir_dynamic )
        if( s.inst )
        {
            this->copy( idx, s );
            idx += 1;
        }
}

template< class Type, size_t LocalItems, size_t LocalStorage >
Sub< Type, LocalItems, LocalStorage > & Sub< Type, LocalItems, LocalStorage >::operator=( Sub< Type, LocalItems, LocalStorage > const & src )
{
    size_t idx = 0;
    
    for( S const & s : src.dir_local )
        if( s.inst )
        {
            this->assign_copy( idx, s );
            idx += 1;
        }
            
    for( S const & s : src.dir_dynamic )
        if( s.inst )
        {
            this->assign_copy( idx, s );
            idx += 1;
        }
}

}
