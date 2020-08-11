namespace iv
{

//==================== PrivField_Owner =================================
template< class T >
void PrivField_Owner< T >::Field_Modify( PrivField< T > * field, T const & val )
{
    field->Modify( val );
}

//==================== LField_RW =================================
template< class T >
PrivField< T >::PrivField( Instance * inst, PrivField_Owner< T > * impl ) :
    Field< T >( inst ),
    impl( impl )
{
}

template< class T >
void PrivField< T >::Modify( T const & val )
{
    this->Field< T >::Modify( val );
}

template< class T >
void PrivField< T >::OnChanged( bool real )
{
    this->impl->Field_OnChanged( this, real );
}

}
