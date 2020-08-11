#include "Context.hpp"

namespace iv
{

Context::Context() :
    ss(),
    mss( &this->ss )
{
}

bool Context::log_enabled( LogId id ) const
{
    return this->log_process_enabled( id );
}

}
