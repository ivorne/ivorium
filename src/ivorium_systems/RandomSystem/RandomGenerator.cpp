#include "RandomGenerator.hpp"

namespace iv
{

Instance * RandomGenerator::instance() const
{
    return this->inst;
}

RandomGenerator::result_type RandomGenerator::operator()()
{
    return this->engine();
}

void RandomGenerator::reset()
{
    this->engine.seed( this->identity_seed );
    this->engine.discard( 5 );
}

}
