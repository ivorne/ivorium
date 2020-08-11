#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class DVarId : public RuntimeId< DVarId >
{
public:
    static constexpr char TypeName[] = "DVarId";
    static RuntimeIdDictionary Dictionary;
    using RuntimeId< DVarId >::RuntimeId;
};

template< class Type >
class DVarIdT : public DVarId
{
public:
    using DVarId::DVarId;
    explicit DVarIdT( DVarId const & id, Type initial ) : DVarId( id ), _initial( initial ){}
    
    Type initial(){ return this->_initial; }
    
private:
    Type _initial;
};

}

namespace std
{
template<>
struct hash< iv::DVarId > : public iv::hash< iv::DVarId >{};
}
