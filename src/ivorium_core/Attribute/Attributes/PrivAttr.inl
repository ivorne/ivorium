namespace iv
{

//--------------------- PrivAttr_Owner -----------------------------------------
template< class T >
void PrivAttr_Owner< T >::SourceValueChanged( PrivAttr_I< T > * attr )
{
    attr->SourceValueChanged();
}

template< class T >
void PrivAttr_Owner< T >::Attribute_SetMode( PrivAttr_I< T > * attr, Attribute::ValueMode mode )
{
    attr->SetAttributeMode( mode );
}

//--------------------- PrivAttr_I -------------------------------------
template< class T >
PrivAttr_I< T >::PrivAttr_I( ClientMarker * cm, AttributeEventProcessor * processor, PrivAttr_Owner< T > * owner ) :
    Attr< T >( cm, processor, false ),
    owner( owner )
{
}

template< class T >
void PrivAttr_I< T >::GetSourceValue( T & out ) const
{
    return this->owner->PrivAttr_Get( out );
}

template< class T >
void PrivAttr_I< T >::ModifySource( T const & value ) const
{
    this->owner->PrivAttr_Modify( value );
}

//--------------------- PrivAttr -------------------------------------
template< class T >
PrivAttr< T >::PrivAttr( ClientMarker * cm, PrivAttr_Owner< T > * owner ) :
    Local_AEP(),
    PrivAttr_I< T >( cm, this, owner )
{
    this->Notify_Activated();
}

template< class T >
PrivAttr< T >::~PrivAttr()
{
    this->Notify_Deactivated();
}

}
