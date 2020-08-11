#pragma once

namespace iv
{

/**
*/
template< class Int >
class SumAgg
{
public:
                        SumAgg();
                        SumAgg( Int aggregated, int size );
    
    SumAgg< Int >       Insert( Int value ) const;
    SumAgg< Int >       Remove( Int value ) const;
    
    Int                 Aggregated() const;
    int                 Size() const;
    
private:
    Int current;
    int count;
};

/**
*/
template< class Int >
struct StringIO< SumAgg< Int > >
{
    SumAgg< Int > Read( const char * source, Context const * context ) const
    {
        return SumAgg< Int >();
    }
    
    std::string Write( SumAgg< Int > const & value, Context const * context ) const
    {
        return StringIO< Int >().Write( value.Aggregated(), context ) + std::string( "[sum of" ) + StringIO< int >().Write( value.Size(), context ) + std::string( "]" );
    }
};

}

#include "SumAgg.inl"
