namespace iv
{

template< class Int >
SumAgg< Int >::SumAgg() :
    current( 0 ),
    count( 0 )
{
}

template< class Int >
SumAgg< Int >::SumAgg( Int current, int count ) :
    current( current ),
    count( count )
{
}

template< class Int >
SumAgg< Int > SumAgg< Int >::Insert( Int value ) const
{
    Int current = this->current + value;
    int count = this->count + 1;
    if( count == 0 )
        current = 0;
    return SumAgg< Int >( current, count );
}

template< class Int >
SumAgg< Int > SumAgg< Int >::Remove( Int value ) const
{
    Int current = this->current - value;
    int count = this->count - 1;
    if( count == 0 )
        current = 0;
    return SumAgg< Int >( current, count );
}

template< class Int >
Int SumAgg< Int >::Aggregated() const
{
    return this->current;
}

template< class Int >
int SumAgg< Int >::Size() const
{
    return this->count;
}

}
