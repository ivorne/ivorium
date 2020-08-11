#include "FontMesh.hpp"

#include "../Defs.hpp"
#include "../Rendering/ColorTransform.hpp"

#include <utf8.h>

namespace iv
{

FontMesh::FontMesh( Instance * inst ) :
    cm( inst, this, "FontMesh" ),
    inst( inst ),
    shader( inst ),
    font( inst ),
    variant_texture( nullptr )
{
    this->cm.owns( this->shader.cm, this->font.cm );
}

Instance * FontMesh::instance() const
{
    return this->inst;
}

void FontMesh::font_path( ResourcePath const & path )
{
    this->font.path( path );
}

GLuint FontMesh::program_id()
{
    return this->shader.get() ? this->shader->program_id() : 0;
}

GLuint FontMesh::texture_id()
{
    if( !this->font.get() )
        return 0;
    
    auto const & variant = this->font->SelectVariantMsdf();
    if( !variant.texture )
        return 0;
    
    return variant.texture->gl_texture()->texture_id();
}

FontMesh::Geometry FontMesh::ComputeGeometry( std::string const & text, float font_size, Location const & location )
{
    if( !this->font.get() )
        return Geometry();
    
    // font data
    Font::Info const & info = this->font->GetInfo();
    float my_ascender = info.ascender * font_size;
    float my_descender = info.descender * font_size;
    
    // surface data
    Geometry geo;
    LineState state = location.line_state;
    
    // loop data
    unsigned skip = location.skip_characters;
    char const * pos = text.c_str();
    char const * const end = pos + text.size();
    while( pos < end )
    {
        uint32_t c = ::utf8::next( pos, end );
        
        if( skip > 0 )
        {
            skip--;
        }
        else
        {
            if( c == uint32_t( '\n' ) )
            {
                // alone in line, but can't fit anyway
                if( !state.baseline_fixed )
                {
                    geo.fits = false;
                    break;
                }
                
                // next line
                geo.max_width = std::max( geo.max_width, state.basepoint.x );
                state.basepoint.y = state.basepoint.y - state.descender + location.line_spacing;
                state.basepoint.x = 0.0f;
                state.preadvance = 0.0f;
                state.baseline_fixed = false;
                
                //
                geo.characters++;
                continue;
            }
            
            auto it_adv = info.advances.find( c );
            if( it_adv == info.advances.end() )
            {
                geo.characters++;
                continue;
            }
            
            auto it_width = info.widths.find( c );
            if( it_width == info.widths.end() )
            {
                geo.characters++;
                continue;
            }
            
            float adv = it_adv->second * font_size;
            float glyph_width = it_width->second * font_size;
            
            // check width, consider newline
            if( state.basepoint.x + state.preadvance + glyph_width > location.size.x )
            {
                // alone in line, but can't fit anyway
                if( !state.baseline_fixed )
                {
                    geo.fits = false;
                    break;
                }
                
                // next line
                geo.max_width = std::max( geo.max_width, state.basepoint.x );
                state.basepoint.y = state.basepoint.y - state.descender + location.line_spacing;
                state.basepoint.x = 0.0f;
                state.preadvance = 0.0f;
                state.baseline_fixed = false;
            }
        
            // fix baseline
            if( !state.baseline_fixed )
            {
                state.ascender = my_ascender;
                state.descender = my_descender;
                state.basepoint.y += state.ascender;
                state.baseline_fixed = true;
            }
            
            // check vertical space
            if( state.basepoint.y - state.descender > location.size.y )
            {
                geo.fits = false;
                break;
            }
            
            // advance by one glyph
            state.basepoint.x += state.preadvance + glyph_width;
            state.preadvance = adv - glyph_width;
        }
        
        geo.characters++;
    }
    
    geo.max_width = std::max( geo.max_width, state.basepoint.x );
    geo.line_state = state;
    
    return geo;
}

void FontMesh::GenerateMesh( std::string const & text, float font_size, Location const & location )
{
    // reset meshes
    this->mesh.CreateMesh( &this->cm );
    this->variant_texture = nullptr;
    
    //
    if( !this->font.get() )
        return;
    
    // font data
    Font::Info const & info = this->font->GetInfo();
    float my_ascender = info.ascender * font_size;
    float my_descender = info.descender * font_size;
    Font::Variant const & variant = this->font->SelectVariantMsdf();
    float variant_size = variant.size;
    float2 tex_size = variant.texture->metadata().size;
    
    this->variant_texture = variant.texture;
    this->variant_texcoordDensity = float2( 1.0f / tex_size.x, 1.0f / tex_size.y ) * variant_size / font_size;
    
    // surface data
    LineState state = location.line_state;
    
    // loop data
    GlMeshData data;
    unsigned skip = location.skip_characters;
    
    this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "GenerateMesh, location.size: ", location.size, " location.line_state.basepoint: ", location.line_state.basepoint );
    
    unsigned next_index = 0;
    
    char const * pos = text.c_str();
    char const * const end = pos + text.size();
    while( pos < end )
    {
        uint32_t c = ::utf8::next( pos, end );
        
        if( skip > 0 )
        {
            skip--;
        }
        else
        {
            this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Glyph ", c, " '", char(c),"'." );
            
            if( c == uint32_t( '\n' ) )
            {
                this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Newline because of newline character." );
                
                // alone in line, but can't fit anyway
                if( !state.baseline_fixed )
                {
                    this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Not enough horizontal space, abort." );
                    break;
                }
                
                // next line
                state.basepoint.y = state.basepoint.y - state.descender + location.line_spacing;
                state.basepoint.x = 0.0f;
                state.preadvance = 0.0f;
                state.baseline_fixed = false;
                
                //
                continue;
            }
            
            auto glyph_it = variant.glyphs.find( c );
            if( glyph_it == variant.glyphs.end() )
            {
                this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Variant not found." );
                continue;
            }
            
            auto it_adv = info.advances.find( c );
            if( it_adv == info.advances.end() )
            {
                this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Advance not found." );
                continue;
            }
            
            auto it_width = info.widths.find( c );
            if( it_width == info.widths.end() )
            {
                this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Glyph width not found." );
                continue;
            }
            
            float adv = it_adv->second * font_size;
            float glyph_width = it_width->second * font_size;
            Font::Glyph const & glyph = glyph_it->second;
            
            // check width, consider newline
            if( state.basepoint.x + state.preadvance + glyph_width > location.size.x )
            {
                this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Newline because basepoint ", state.basepoint.x, " remainder advance ", state.preadvance, " and glyph width ", glyph_width, " exceed available size ", location.size.x, "." );
                
                // alone in line, but can't fit anyway
                if( !state.baseline_fixed )
                {
                    this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Not enough horizontal space, abort." );
                    break;
                }
                
                // next line
                state.basepoint.y = state.basepoint.y - state.descender + location.line_spacing;
                state.basepoint.x = 0.0f;
                state.preadvance = 0.0f;
                state.baseline_fixed = false;
            }
            
            // fix baseline
            if( !state.baseline_fixed )
            {
                state.ascender = my_ascender;
                state.descender = my_descender;
                state.basepoint.y += state.ascender;
                state.baseline_fixed = true;
            }
            
            // check vertical space
            if( state.basepoint.y - state.descender > location.size.y )
            {
                this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Not enough vertical space, abort." );
                break;
            }
        
            // add glyph to mesh
            float pos_left = state.basepoint.x + state.preadvance + glyph.bearing_x * font_size / variant_size;
            float pos_top = state.basepoint.y - glyph.bearing_y * font_size / variant_size;
            float pos_right = pos_left + float( glyph.width ) * font_size / variant_size;
            float pos_bottom = pos_top + float( glyph.height ) * font_size / variant_size;
            
            float tex_left = float( glyph.pos_x ) / tex_size.x;
            float tex_top = float( glyph.pos_y ) / tex_size.y;
            float tex_right = tex_left + float( glyph.width ) / tex_size.x;
            float tex_bottom = tex_top + float( glyph.height ) / tex_size.y;
            
            tex_top = 1.0f - tex_top;
            tex_bottom = 1.0f - tex_bottom;
            
            // ( 0, 0 )
            data.positions.push_back( pos_left );
            data.positions.push_back( pos_top );
            data.positions.push_back( 0 );
            
            data.texcoords.push_back( tex_left );
            data.texcoords.push_back( tex_top );
            
            data.indices.push_back( next_index++ );
            
            // ( 0, 1 )
            data.positions.push_back( pos_left );
            data.positions.push_back( pos_bottom );
            data.positions.push_back( 0 );
            
            data.texcoords.push_back( tex_left );
            data.texcoords.push_back( tex_bottom );
            
            data.indices.push_back( next_index++ );
            
            // ( 1, 0 )
            data.positions.push_back( pos_right );
            data.positions.push_back( pos_top );
            data.positions.push_back( 0 );
            
            data.texcoords.push_back( tex_right );
            data.texcoords.push_back( tex_top );
            
            data.indices.push_back( next_index++ );
            
            // ( 1, 1 )
            data.positions.push_back( pos_right );
            data.positions.push_back( pos_bottom );
            data.positions.push_back( 0 );
            
            data.texcoords.push_back( tex_right );
            data.texcoords.push_back( tex_bottom );
            
            data.indices.push_back( next_index++ );
            
            // primitive restart
            data.indices.push_back( GlMesh::PrimitiveRestart );
            
            //
            this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Glyph generated on ( ", pos_left, ", ", pos_top, " ) - ( ", pos_right, ", ", pos_bottom, " )", Context::Endl(),
                                                                "with texcoords ( ", tex_left, ", ", tex_top, " ) - ( ", tex_right, ", ", tex_bottom, " )." );
            
            // advance
            this->cm.log( SRC_INFO, Defs::Log::Text_Verbose, "Advance by remainder ", state.preadvance, " and glyph width ", glyph_width, "."  );
            state.basepoint.x += state.preadvance + glyph_width;
            state.preadvance = adv - glyph_width;
        }
    }
    
    data.draw_mode = GL_TRIANGLE_STRIP;
    this->mesh.Load_All( &this->cm, data );
}

void FontMesh::Render( CameraState const & camera, float4x4 const & model_transform, float4 color, float4 preblend, bool translucent, std::optional< float > depth_override, ShaderScissor const & scissor )
{
    FlatShader::Params params( &this->cm );
    params.filteringStage = FlatShader::FilteringStage::Msdf;
    params.alpha.Set( color.a );
    params.colorTransform.Set( ColorTransform::Shift( float4( color.r, color.g, color.b, 1.0f ) ) * ColorTransform::Zero() );
    
    this->shader->Render(
        this->cm,
        
        camera,
        depth_override,
        
        model_transform,
        scissor,
        
        preblend,
        translucent,
        
        &this->mesh,
        float3( 1, 1, 1 ),
        this->variant_texcoordDensity,
        
        this->variant_texture->gl_texture(),
        1.0f,
        this->variant_texture->metadata().msdf_pixelRange,
        
        params
    );
}

}
