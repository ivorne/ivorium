#include "LumaSystem.hpp"
#include "Defs.hpp"

namespace iv
{

//-------------- LumaStyleId -------------------------
iv::RuntimeIdDictionary LumaStyleId::Dictionary;

//----------------------------
LumaStyle::LumaStyle() :
    surface( 1, 1, 1, 1 ),
    highlight( 0.384, 0, 0.93, 1 ),
    error( 0.69, 0, 0.13, 1 ),
    on_surface( 0, 0, 0, 1 ),
    on_highlight( 1, 1, 1, 1 ),
    on_error( 1, 1, 1, 1 ),
    font_normal( "/ivorium_UI/fonts/DejaVuSansMono/font.index" ),
    font_monospace( "/ivorium_UI/fonts/DejaVuSansMono/font.index" )
{
}

//------------- LumaListener ------------------------------------
LumaListener::LumaListener( Instance * inst, LumaStyleId id ) :
    cm( inst, this, "LumaListener" ),
    inst( inst ),
    id( id )
{
    auto ls = this->inst->getSystem< LumaSystem >();
    if( ls )
    {
        ls->AddListener( this );
        if( !this->id.valid() )
            this->id = ls->current_style();
    }
}

LumaListener::~LumaListener()
{
    auto ls = this->inst->getSystem< LumaSystem >();
    if( ls )
        ls->RemoveListener( this );
}

Instance * LumaListener::instance() const
{
    return this->inst;
}

LumaStyleId LumaListener::style_id() const
{
    return this->id;
}


LumaStyle const & LumaListener::style() const
{
    auto ls = this->inst->getSystem< LumaSystem >();
    if( !ls )
    {
        static LumaStyle _v;
        return _v;
    }
        
    return ls->style( this->id );
}

//------------- LumaStyler ------------------------------------
LumaStyler::LumaStyler( Instance * inst ) :
    cm( inst, this, "LumaStyler" ),
    inst( inst ),
    ls( this->instance()->getSystem< LumaSystem >() )
{
}

Instance * LumaStyler::instance() const
{
    return this->inst;
}

void LumaStyler::current_style( LumaStyleId id )
{
    if( !this->ls )
        return;
    this->ls->current_style( id );
}

LumaStyleId LumaStyler::current_style() const
{
    if( !this->ls )
        return LumaStyleId();
    return this->ls->current_style();
}

void LumaStyler::style( LumaStyleId id, LumaStyle const & style )
{
    if( !this->ls )
        return;
    
    if( !id.valid() )
    {
        this->cm.warning( SRC_INFO, "Invalid LumaStyleId can not be set. Setting Default StyleId instead." );
        id = Defs::Style::Default;
    }
    
    this->ls->style( id, style );
}

LumaStyle const & LumaStyler::style( LumaStyleId id ) const
{
    if( !this->ls )
    {
        static LumaStyle _v;
        return _v;
    }
    return this->ls->style( id );
}

//------------- LumaSystem ------------------------------------
LumaSystem::LumaSystem( SystemContainer * sc ) :
    iv::System( sc ),
    _current_style( Defs::Style::Default )
{
    // default style colors
    LumaStyle style;
    this->_styles[ this->_current_style ] = style;
}

void LumaSystem::current_style( LumaStyleId id )
{
    this->_current_style = id;
}

LumaStyleId LumaSystem::current_style() const
{
    return this->_current_style;
}

void LumaSystem::style( LumaStyleId id, LumaStyle const & style )
{
    LumaStyle & stored_style = this->_styles[ id ];
    stored_style = style;
    this->_listeners.foreach( 
        [&]( LumaListener * listener )
        {
            if( listener->style_id() == id )
                listener->LumaStyleChanged( stored_style );
        }
    );
}

LumaStyle const & LumaSystem::style( LumaStyleId id ) const
{
    auto it = this->_styles.find( id );
    if( it == this->_styles.end() )
        return this->_styles.at( Defs::Style::Default );
    return it->second;
}

void LumaSystem::AddListener( LumaListener * listener )
{
    this->_listeners.erase( listener );
}

void LumaSystem::RemoveListener( LumaListener * listener )
{
    this->_listeners.erase( listener );
}

}
