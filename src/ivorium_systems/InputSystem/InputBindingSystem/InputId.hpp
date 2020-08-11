#pragma once 

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class InputId final : public RuntimeId< InputId >
{
public:
    static constexpr char TypeName[] = "InputId";
    static RuntimeIdDictionary Dictionary;
    using RuntimeId< InputId >::RuntimeId;
};

}

namespace std
{
template<> struct hash< iv::InputId > : public iv::hash< iv::InputId >{};
}
