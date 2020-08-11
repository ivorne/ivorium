#include "Font.hpp"

namespace iv
{

Font::Font( Instance * inst ) :
    cm( inst, this, "Font" ),
    inst( inst )
{
}

Instance * Font::instance() const
{
    return this->inst;
}

Font::Info const & Font::GetInfo() const
{
    return this->_info;
}

Font::Variant const & Font::SelectVariant( unsigned font_size ) const
{
    auto it = this->_variant_fixed.find( font_size );
    if( it != this->_variant_fixed.end() )
        return it->second;
    else
        return this->_variant_msdf;
}

Font::Variant const & Font::SelectVariantMsdf() const
{
    return this->_variant_msdf;
}

}
