#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
*/
class RandomId : public RuntimeId< RandomId >
{
public:
    static constexpr char TypeName[] = "RandomId";
    static RuntimeIdDictionary Dictionary;
    using RuntimeId< RandomId >::RuntimeId;
};

}

namespace std
{
template<>
struct hash< iv::RandomId > : public iv::hash< iv::RandomId >{};
}

