#pragma once

#include "RandomSystem.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <random>

namespace iv
{

/**
    Meant to be used with standard library Distributions from std::random, but operator() can be used as well.
    Each instantiation of this resets the sequence.
    The sequence starts on value determined by seed of RandomId stored in RandomSystem and hash of identity given in constructor.
    If we want to generate one value, we need to instantiate RandomGenerator and use it once.
    If we want to generate a sequence, we need to keep it instantiated and keep using it.
    If we want a seqnece but are unable to keep it instantiated, we can pass the order of generated number as part of the identity.
*/
class RandomGenerator
{
public:
                                    template< class T >
                                    RandomGenerator( Instance * inst, RandomId id, T const & identity );
    Instance *                      instance() const;
    
    void                            reset();
    
    // UniformRandomBitGenerator
    using                           result_type = uint_fast32_t;
    result_type                     operator()();
    static constexpr result_type    max() { return std::minstd_rand::max(); }
    static constexpr result_type    min() { return std::minstd_rand::min(); }
    
private:
    Instance * inst;
    std::minstd_rand engine;
    
    size_t identity_seed;
};

}

#include "RandomGenerator.inl"
