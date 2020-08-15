#include "Link.hpp"

namespace iv
{

VLink::VLink() :
    _type( typeid( void ) ),
    _target( nullptr )
{
}

std::type_index VLink::type()
{
    return this->_type;
}

}
