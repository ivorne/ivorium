namespace iv
{

//========================= SharedAttr_I =========================================
template< class T >
SharedAttr_I< T >::SharedAttr_I( ClientMarker * cm, AttributeEventProcessor * processor, T const & initial_value ) :
    Attr< T >( cm, processor, true ),
    value( initial_value )
{
}

template< class T >
void SharedAttr_I< T >::GetSourceValue( T & out ) const
{
    out = this->value;
}

template< class T >
void SharedAttr_I< T >::ModifySource( T const & val )
{
    this->value = val;
    this->SourceValueChanged();
}

//========================= SharedAttr =========================================
template< class T >
SharedAttr< T >::SharedAttr( ClientMarker * cm, T const & initial_value ) :
    Local_AEP(),
    SharedAttr_I< T >( cm, this, initial_value )
{
    this->Notify_Activated();
}

template< class T >
SharedAttr< T >::~SharedAttr()
{
    this->Notify_Deactivated();
}



}
