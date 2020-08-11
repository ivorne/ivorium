namespace iv
{

template< class T >
Attr< T >::Attr( ClientMarker * cm, AttributeEventProcessor * processor, bool allow_ModifyMode ) :
    Attribute( cm, processor, allow_ModifyMode )
{
}

template< class T >
T Attr< T >::Get() const
{
    T result;
    this->GetSourceValue( result );
    return result;
}

template< class T >
void Attr< T >::Modify( ClientMarker * modifier, T const & val )
{
    if( this->Mode() != Attribute::ValueMode::ValueModify )
    {
        if( this->allow_ModifyMode() )
        {
            this->SetAttributeMode( Attribute::ValueMode::ValueModify );
        }
        else
        {
            this->owner()->warning( SRC_INFO, "Can not modify Attribute value - ValueMode is ", this->Mode(), "." );
            return;
        }
    }
    
    this->ModifySource( val );
}

template< class T >
std::type_index Attr< T >::Type_Impl()
{
    return typeid( T );
}

template< class T >
std::any Attr< T >::GetAny_Impl()
{
    return std::any( this->Get() );
}

template< class T >
void Attr< T >::ModifyAny_Impl( ClientMarker * modifier, std::any val )
{
    if( val.type() != typeid( T ) )
    {
        this->owner()->warning( SRC_INFO, "Can not modify Attribute with an any value - Attribute type is ", this->Type().name(), " but value type is ", val.type().name(), "." );
        return;
    }
    
    this->Modify( modifier, std::any_cast< T >( val ) );
}

}
