namespace iv
{

//=========================== Field ==================================
template< class T >
Field< T >::Field( Instance * inst ) :
    FieldI(),
    AttributeListener(),
    cm( inst, this, "Field_R" ),
    inst( inst ),
    source_r( nullptr ),
    source_rw( nullptr )
{
}

template< class T >
Field< T >::~Field()
{
    if( this->source_r )
        this->source_r->Remove_Listener( this );
}

template< class T >
Instance * Field< T >::instance() const
{
    return this->inst;
}

template< class T >
void Field< T >::Assign_Attribute_R( Attr< T > * src )
{
    if( this->source_r )
        this->source_r->Remove_Listener( this );
    
    this->constant = {};
    this->source_r = src;
    this->source_rw = nullptr;
    
    if( this->source_r )
        this->source_r->Insert_Listener( this );
    
    this->OnChanged( false );
}

template< class T >
void Field< T >::Release_Attribute()
{
    if( this->source_r )
        this->source_r->Remove_Listener( this );
    
    this->source_r = nullptr;
    this->source_rw = nullptr;
    this->constant = {};
    
    this->OnChanged( false );
}

template< class T >
void Field< T >::Assign_Constant( T const & value )
{
    if( this->constant == value )
        return;
    
    if( this->source_r )
        this->source_r->Remove_Listener( this );
    
    this->source_r = nullptr;
    this->source_rw = nullptr;
    this->constant = value;
    
    this->OnChanged( false );
}

template< class T >
T Field< T >::Get() const
{
    if( this->constant.has_value() )
        return this->constant.value();
    
    if( !this->source_r )
        return T();
    
    auto result = this->source_r->Get();
    return result;
}

template< class T >
void Field< T >::Attribute_Changed( Attribute * src )
{
    this->OnChanged( true );
}

template< class T >
void Field< T >::Attribute_Deactivated( Attribute * src )
{
    this->source_r = nullptr;
    this->source_rw = nullptr;
    this->OnChanged( false );
}

template< class T >
void Field< T >::AnySource_R_Impl( Attribute * src )
{
    Attr< T > * casted = dynamic_cast< Attr< T > * >( src );
    if( casted )
        this->Assign_Attribute_R( casted );
}

template< class T >
std::type_index Field< T >::Type_Impl()
{
    return typeid( T );
}

template< class T >
Attribute::ValueMode Field< T >::mode_Impl() const
{
    if( this->source_rw )
    {
        return this->source_rw->Mode();
    }
    else if( this->source_r )
    {
        auto src_mode = this->source_r->Mode();
        if( src_mode == Attribute::ValueMode::Disabled )
            return Attribute::ValueMode::Disabled;
        else
            return Attribute::ValueMode::Value;
    }
    else if( this->constant.has_value() )
    {
        return Attribute::ValueMode::Value;
    }
    else
    {
        return Attribute::ValueMode::Disabled;
    }
}

template< class T >
FieldI::Assignment Field< T >::AssignmentState_impl() const
{
    if( this->source_rw )
        return Assignment::Attribute_RW;
    else if( this->source_r )
        return Assignment::Attribute_R;
    else if( this->constant.has_value() )
        return Assignment::Constant;
    else
        return Assignment::Unassigned;
}

template< class T >
void Field< T >::Assign_Attribute_RW( Attr< T > * attr )
{
    if( this->source_r )
        this->source_r->Remove_Listener( this );
    
    this->constant = {};
    this->source_r = attr;
    this->source_rw = attr;
    
    if( this->source_r )
        this->source_r->Insert_Listener( this );
    
    this->OnChanged( false );
}

template< class T >
void Field< T >::Modify( T const & val )
{
    if( this->source_rw )
        this->source_rw->Modify( &this->cm, val );
}

template< class T >
void Field< T >::AnySource_RW_Impl( Attribute * src )
{
    Attr< T > * casted = dynamic_cast< Attr< T > * >( src );
    if( casted )
        this->Assign_Attribute_RW( casted );
}

}
