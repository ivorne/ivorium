#pragma once

#include "Font.hpp"

#include "../Rendering/FlatShader.hpp"
#include "../Elements/ElementRenderer.hpp"

namespace iv
{

/**
*/
class FontMesh
{
public:
    struct LineState
    {
        float2      basepoint;
        float       preadvance;         ///< This advance should be done before adding another glyph.
        
        bool        baseline_fixed;     ///< If this is false, then ascender and descender is irrelevant and basepoint is top left of available space (so next segment can set ascender or descender and set basepoint on position basepoint.y+ascender).
        float       ascender;
        float       descender;
        
        LineState() : basepoint( 0, 0 ), preadvance( 0 ), baseline_fixed( false ), ascender( 0 ), descender( 0 ){}
        bool operator==( LineState const & b ) { return this->basepoint==b.basepoint && this->baseline_fixed==b.baseline_fixed && this->ascender==b.ascender && this->descender==b.descender; }
    };
    
    struct Location
    {
        float2      size;
        float       line_spacing;       ///< Lines in inner block should be this much pixels apart (distance of baselines is ascender - descender + line_spacing).
        
        LineState   line_state;
        
        unsigned    skip_characters;
        
        Location() : size( 0, 0 ), line_spacing( 0 ), line_state(), skip_characters( 0 ){}
        bool operator==( Location const & b ) { return this->size==b.size && this->line_spacing==b.line_spacing && this->line_state==b.line_state && this->skip_characters==b.skip_characters; }
    };
    
    struct Geometry
    {
        unsigned    characters;         ///< How many characters fit into available space. Includes characters skipped according to Location::skip_characters.
        bool        fits;               ///< If everything that was meant to fit into layout fits there. If this is false, we will not show any more TextSegments after this one.
        float       max_width;
        
        LineState   line_state;
        
        Geometry() : characters( 0 ), fits( true ), max_width( 0 ), line_state(){}
    };
    
public:
ClientMarker cm;

                            FontMesh( Instance * inst );
    Instance *              instance() const;
    
    void                    font_path( ResourcePath const & path );
    
    GLuint                  program_id();
    GLuint                  texture_id();
    
    Geometry                ComputeGeometry( std::string const & text, float font_size, Location const & location );
    void                    GenerateMesh( std::string const & text, float font_size, Location const & location );
    void                    Render( CameraState const & camera, float4x4 const & model_transform, float4 color, float4 preblend, bool translucent, std::optional< float > depth_override, ShaderScissor const & scissor );
    
private:
    Instance * inst;
    FlatShader_Resource shader;
    Font_Resource font;
    
    Texture const * variant_texture;
    float2 variant_texcoordDensity;
    GlMesh mesh;
};

}
