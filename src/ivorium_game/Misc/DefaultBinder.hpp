#pragma once

#include <ivorium_UI/ivorium_UI.hpp>
#include "../Defs.hpp"

namespace iv
{

/**
    When created, binds all InputIds defined in <ivorium_UI/Defs.hpp>.
*/
class DefaultBinder : public InputBinder
{
public:
    ClientMarker cm;
    
    DefaultBinder( Instance * inst ) :
        InputBinder( inst ),
        cm( inst, this, "DefaultBinder" )
    {
        this->cm.inherits( this->InputBinder::cm );
        
        //
        this->BindKey( Defs::Input::UI_Up, Input::Key::Up );
        this->BindKey( Defs::Input::UI_Down, Input::Key::Down );
        this->BindKey( Defs::Input::UI_Left, Input::Key::Left );
        this->BindKey( Defs::Input::UI_Right, Input::Key::Right );
        
        this->BindKey( Defs::Input::UI_MousePrimary, Input::Key::MouseLeft );
        this->BindKey( Defs::Input::UI_MousePrimary, Input::Key::Touch );
        this->BindHoverKey( Defs::Input::UI_MousePrimary, Input::Key::Mouse );
    }
};


}
