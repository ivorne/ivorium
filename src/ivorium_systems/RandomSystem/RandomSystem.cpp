#include "RandomSystem.hpp"

namespace iv
{

RandomSystem::RandomSystem( SystemContainer * sc ) :
    System( sc )
{
}

void RandomSystem::seed( RandomId id, uint_fast32_t seed )
{
    this->seeds[ id ] = seed;
}

uint_fast32_t RandomSystem::seed( RandomId id )
{
    auto it = this->seeds.find( id );
    if( it == this->seeds.end() )
        return 0;
    return it->second;
}

}
