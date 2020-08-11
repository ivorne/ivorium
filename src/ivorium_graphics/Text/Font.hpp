#pragma once

#include <ivorium_core/ivorium_core.hpp>
#include "../Rendering/Texture.hpp"
#include <unordered_map>

namespace iv
{

/**
*/
class Font
{
public:
ClientMarker cm;
    
    enum class Type
    {
        Hinted,
        Msdf,
    };
    
    struct Info
    {
        float line_height;  ///< Relative to font size. From one baseline to another baseline.
        float ascender;     ///< Relative to font size. From baseline to top of highest glyph.
        float descender;    ///< Relative to font size. From baseline to bottom of most bottom-reaching glyph.
        float max_advance;  ///< Relative to font size. Maximal advance of a glyph.
        
        std::unordered_map< uint32_t, float > advances; ///< Relative to font size. Advance after given glyph.
        std::unordered_map< uint32_t, float > widths;   ///< Relative to font size. Distance from basepoint to right side of the glyph.
        
        Info() : line_height( 0 ), ascender( 0 ), descender( 0 ), max_advance( 0 ){}
    };
    
    struct Glyph
    {
        uint32_t code;      ///< glyph (unicode code)
        unsigned pos_x;     ///< x position of glyph in bitmap (from left of image to left of glyph, in texture pixels)
        unsigned pos_y;     ///< y position of glyph in bitmap (from top of image to top of glyph, in texture pixels)
        unsigned width;     ///< width of glyph in pixels (from left of glyph to right of glyph, in texture pixels)
        unsigned height;    ///< height of glyph in pixels (from top of glyph to bottom of glyph, in texture pixels)
        float bearing_x;    ///< from base of glyph to left side of glyph, in texture pixels
        float bearing_y;    ///< from base of glyph to top side of glyph, in texture pixels
        
        Glyph() : code( 0 ){}
    };
    
    struct Variant
    {
        Texture const * texture;
        Type            type;
        unsigned        size;               ///< Font size.
        
        std::unordered_map< uint32_t, Glyph > glyphs;
        
        Variant() : texture( nullptr ), type( Type::Hinted ){}
    };
    
                            Font( Instance * inst );
    Instance *              instance() const;
    
    /**
    */
    Info const &            GetInfo() const;
    
    /**
        If hinted font with this size exists, this returns it, otherwise it returns msdf.
    */
    Variant const &         SelectVariant( unsigned font_size ) const;
    
    /**
    */
    Variant const &         SelectVariantMsdf() const;
    
protected:
    Info _info;
    std::unordered_map< unsigned, Variant > _variant_fixed;
    Variant _variant_msdf;
    
private:
    Instance * inst;
};


class Font_Resource : public SingularResource< Font >
{
public:
ClientMarker cm;
    Font_Resource( Instance * inst, ResourcePath const & path = ResourcePath() ) :
        SingularResource< Font >( inst, path ),
        cm( inst, this, "Font_Resource" )
    {
        this->cm.inherits( this->SingularResource< Font >::cm );
    }
};


}
