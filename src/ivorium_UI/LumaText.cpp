#include "LumaText.hpp"

namespace iv
{

LumaText::LumaText( Instance * inst, LumaStyleId style_id, bool monospace ) :
    Text( inst ),
    LumaListener( inst, style_id ),
    cm( inst, this, "LumaText" ),
    monospace( monospace )
{
    this->cm.inherits( this->Text::cm, this->LumaListener::cm );
    this->LumaStyleChanged( this->style() );
}

void LumaText::LumaStyleChanged( LumaStyle const & style )
{
    this->color( style.on_surface );
    
    if( this->monospace )
        this->font( style.font_monospace );
    else
        this->font( style.font_normal );
}

}
