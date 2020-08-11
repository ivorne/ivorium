#pragma once


#include <string>
#include <vector>
#include "../Basics/Context.hpp"

namespace iv
{

/**
*/
class DebugView
{
public:
                    DebugView( Context * context );
    Context *       context() const;
    
private:
    Context * _context;
};

}
