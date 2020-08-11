#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class TimeId : public RuntimeId< TimeId >
{
public:
    static constexpr char TypeName[] = "TimeId";
    static RuntimeIdDictionary Dictionary;
    using RuntimeId< TimeId >::RuntimeId;
};

}
