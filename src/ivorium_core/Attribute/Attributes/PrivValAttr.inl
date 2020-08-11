namespace iv
{

//----------------- PrivValAttr_Owner -----------------------

template< class T >
void PrivValAttr_Owner::Attribute_Set( PrivValAttr_I< T > * attr, T const & val )
{
    attr->Set( val );
}

template< class T >
void PrivValAttr_Owner::Attribute_SetMode( PrivValAttr_I< T > * attr, Attribute::ValueMode mode )
{
    attr->SetAttributeMode( mode );
}

//---------------- PrivValAttr_I -----------------------------
template< class T >
PrivValAttr_I< T >::PrivValAttr_I( ClientMarker * cm, AttributeEventProcessor * processor, PrivValAttr_Owner * owner, T const & initial_value ) :
    Attr< T >( cm, processor, false ),
    owner( owner ),
    value( initial_value )
{
}

template< class T >
void PrivValAttr_I< T >::GetSourceValue( T & out ) const
{
    out = this->value;
}

template< class T >
void PrivValAttr_I< T >::ModifySource( T const & new_value )
{
    this->value = new_value;
    this->owner->PrivValAttr_Modified( this );
    this->SourceValueChanged();
}

template< class T >
void PrivValAttr_I< T >::Set( T const & value )
{
    this->value = value;
    this->owner->PrivValAttr_Modified( this );
    this->SourceValueChanged();
}

//---------------- PrivValAttr -----------------------------
template< class T >
PrivValAttr< T >::PrivValAttr( ClientMarker * cm, PrivValAttr_Owner * owner, T const & initial_value ) :
    Local_AEP(),
    PrivValAttr_I< T >( cm, this, owner, initial_value )
{
    this->Notify_Activated();
}

template< class T >
PrivValAttr< T >::~PrivValAttr()
{
    this->Notify_Deactivated();
}

}
