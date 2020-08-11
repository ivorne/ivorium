namespace iv
{

//-------------------------------
template< class T >
DirtyAttr_I< T >::DirtyAttr_I( ClientMarker * cm, AttributeEventProcessor * processor, T const & initial_value ) :
    Attr< T >( cm, processor, false ),
    _value( initial_value ),
    _dirty( true )
{
    this->SetAttributeMode( Attribute::ValueMode::ValueModify );
}


template< class T >
bool DirtyAttr_I< T >::operator<( DirtyAttr_I< T > const & right ) const
{
    return std::tie( this->_value, this->_dirty ) < std::tie( right._value, right._dirty );
}

template< class T >
bool DirtyAttr_I< T >::clear_dirty()
{
    bool result = this->_dirty;
    this->_dirty = false;
    return result;
}

template< class T >
bool DirtyAttr_I< T >::dirty() const
{
    return this->_dirty;
}

template< class T >
void DirtyAttr_I< T >::GetSourceValue( T & out ) const
{
    out = this->_value;
}

template< class T >
void DirtyAttr_I< T >::ModifySource( T const & val )
{
    if( this->_value == val )
        return;
    
    this->_value = val;
    this->_dirty = true;
    this->Attr< T >::SourceValueChanged();
}

template< class T >
void DirtyAttr_I< T >::Set( T const & val )
{
    this->ModifySource( val );
}

//-----------------------
template< class T >
DirtyAttr< T >::DirtyAttr( ClientMarker * cm, T const & initial_value ) :
    Local_AEP(),
    DirtyAttr_I< T >( cm, this, initial_value )
{
    this->Notify_Activated();
}

template< class T >
DirtyAttr< T >::~DirtyAttr()
{
    this->Notify_Deactivated();
}

}
