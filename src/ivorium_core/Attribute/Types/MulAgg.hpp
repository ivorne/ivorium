#pragma once

namespace iv
{

/**
*/
template< class Float >
class MulAgg
{
public:
                        MulAgg();
                        MulAgg( Float aggregated, int size );
    
    MulAgg< Float >     Insert( Float value ) const;
    MulAgg< Float >     Remove( Float value ) const;
    
    Float               Aggregated() const;
    int                 Size() const;
    
private:
    Float current;
    int count;
};

/**
*/
template< class Float >
struct StringIO< MulAgg< Float > >
{
    MulAgg< Float > Read( const char * source, Context const * context ) const
    {
        return MulAgg< Float >();
    }
    
    std::string Write( MulAgg< Float > const & value, Context const * context ) const
    {
        return StringIO< Float >().Write( value.Aggregated(), context ) + std::string( "[mul of " ) + StringIO< int >().Write( value.Size(), context ) + std::string( "]" );
    }
};

}

#include "MulAgg.inl"
