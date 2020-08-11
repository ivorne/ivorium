namespace iv
{

template< class Float >
MulAgg< Float >::MulAgg() :
    current( 1.0 ),
    count( 0 )
{
}

template< class Float >
MulAgg< Float >::MulAgg( Float current, int count ) :
    current( current ),
    count( count )
{
}

template< class Float >
MulAgg< Float > MulAgg< Float >::Insert( Float value ) const
{
    Float current = this->current * value;
    int count = this->count + 1;
    if( count == 0 )
        current = 1.0;
    return MulAgg< Float >( current, count );
}

template< class Float >
MulAgg< Float > MulAgg< Float >::Remove( Float value ) const
{
    Float current = this->current / value;
    int count = this->count - 1;
    if( count == 0 )
        current = 1.0;
    return MulAgg< Float >( current, count );
}

template< class Float >
Float MulAgg< Float >::Aggregated() const
{
    return this->current;
}

template< class Float >
int MulAgg< Float >::Size() const
{
    return this->count;
}

}
