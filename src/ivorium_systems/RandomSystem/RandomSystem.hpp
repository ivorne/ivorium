#pragma once

#include "RandomId.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
*/
class RandomSystem : public System
{
public:
                            RandomSystem( SystemContainer * sc );
    virtual std::string     debug_name() const override { return "RandomSystem"; }
    
    void                    seed( RandomId, uint_fast32_t seed );
    uint_fast32_t           seed( RandomId );
    
private:
    std::unordered_map< RandomId, uint_fast32_t > seeds;
};

}
