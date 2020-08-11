#pragma once

#include "RuntimeId.hpp"

namespace iv
{

/**
*/
class LogId : public RuntimeId< LogId >
{
public:
    static constexpr char TypeName[] = "LogId";
    static RuntimeIdDictionary Dictionary;
    using RuntimeId< LogId >::RuntimeId;
};

}

namespace std
{

template<>
struct hash< iv::LogId > : public iv::hash< iv::LogId >{};

}
