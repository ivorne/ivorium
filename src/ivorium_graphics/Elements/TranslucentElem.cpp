#include "TranslucentElem.hpp"

namespace iv
{

TranslucentElem::TranslucentElem( Elem * elem ) :
    cm( elem->instance(), this, "TranslucentElem", ClientMarker::Status() ),
    attr_preblend( &this->cm, float4( 0, 0, 0, 0 ) ),
    attr_translucent( &this->cm, false ),
    _elem( elem )
{
}

void TranslucentElem::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "TranslucentElem" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "preblend", this->attr_preblend.Get() );
    row.Column( "translucent", this->attr_translucent.Get() );
}

Elem * TranslucentElem::elem()
{
    return this->_elem;
}

Elem const * TranslucentElem::elem() const
{
    return this->_elem;
}

TranslucentElem * TranslucentElem::preblend( float4 val )
{
    this->attr_preblend.Set( val );
    return this;
}

TranslucentElem * TranslucentElem::translucent( bool val )
{
    this->attr_translucent.Set( val );
    return this;
}

}
