#include "FixedOrder_Camera.hpp"
#include "../Defs.hpp"

namespace iv
{

bool FixedOrder_Camera::RenderIdComparator::operator()( RenderId const & left, RenderId const & right ) const
{
    return std::tie( left.shader_id, left.primary_texture_id, left.renderable ) < std::tie( right.shader_id, right.primary_texture_id, right.renderable );
}

FixedOrder_Camera::FixedOrder_Camera( Instance * inst, iv::RenderTarget::Geometry geometry ) :
    Camera( inst, geometry ),
    cm( inst, this, "FixedOrder_Camera" ),
    _depth_next( 0 )
{
    this->cm.inherits( this->Camera::cm );
}

void FixedOrder_Camera::FrameStart()
{
    this->translucents.clear();
    this->_depth_next = 0;
}

void FixedOrder_Camera::AddRenderable_Solid( Renderable * renderable, GLuint shader_id, GLuint primary_texture_id )
{
    this->cm.log( SRC_INFO, Defs::Log::FixedOrderCamera, "Add solid (depth ",this->_depth_next++,")." );
    
    RenderId id;
    id.renderable = renderable;
    id.shader_id = shader_id;
    id.primary_texture_id = primary_texture_id;
    
    SolidInfo info;
    info.frame_id = this->instance()->frame_id();
    info.depth = this->_depth_next++;
    
    this->solids[ id ] = info;
}

void FixedOrder_Camera::AddRenderable_Translucent( Renderable * renderable, GLuint shader_id, GLuint primary_texture_id )
{
    this->cm.log( SRC_INFO, Defs::Log::FixedOrderCamera, "Add translucent (depth ",this->_depth_next++,")." );
    
    RenderId id;
    id.renderable = renderable;
    id.shader_id = shader_id;
    id.primary_texture_id = primary_texture_id;
    
    TranslucentInfo info;
    info.id = id;
    info.depth = this->_depth_next++;
    
    this->translucents.push_back( info );
}

void FixedOrder_Camera::RunRender( CameraState const & camera )
{
    //
    auto frame_id = this->instance()->frame_id();
    RenderTarget * target = GlInfo( this->instance() ).render_target();
    
    // stats
    int solid_throws = 0;
    int solid_renders = 0;
    int translucent_renders = 0;
    int shader_switches = 0;
    int texture_switches = 0;
    RenderId previous_id;
    
    // render solid targets
    {
        // enable depth test
        target->DepthBufferMode( GL_GEQUAL, true );
        target->DepthBufferClear( 0.0f );
        target->Blending( false );
        
        //
        this->cm.log( SRC_INFO, Defs::Log::FixedOrderCamera, "Render ", this->solids.size(), " solids." );
        
        // render
        auto it = this->solids.begin();
        while( it != this->solids.end() )
        {
            RenderId const & id = it->first;
            SolidInfo const & info = it->second;
            
            // throw away obsolete target
            if( info.frame_id != frame_id )
            {
                //
                solid_throws++;
                
                //
                auto todel = it;
                ++it;
                this->solids.erase( todel );
                continue;
            }
            
            // check context switches
            if( id.shader_id != previous_id.shader_id )
                shader_switches++;
            if( id.primary_texture_id != previous_id.primary_texture_id )
                texture_switches++;
            previous_id = id;
            
            // render
            solid_renders++;
            id.renderable->render( camera, float( info.depth + 1 ) / float( this->_depth_next + 2 ) );
            
            //
            ++it;
        }
    }
    
    // render translucent targets
    {
        // disable depth test
        target->DepthBufferMode( GL_GEQUAL, false );
        target->Blending( true );
        
        //
        this->cm.log( SRC_INFO, Defs::Log::FixedOrderCamera, "Render ", this->translucents.size(), " translucents." );
        
        // render
        for( TranslucentInfo const & info : this->translucents )
        {
            RenderId const & id = info.id;
            
            // check context switches
            if( id.shader_id != previous_id.shader_id )
                shader_switches++;
            if( id.primary_texture_id != previous_id.primary_texture_id )
                texture_switches++;
            previous_id = id;
            
            // render
            id.renderable->render( camera, float( info.depth + 1 ) / float( this->_depth_next + 2 ) );
            translucent_renders++;
        }
    }
    
    // print stats
    this->cm.log( SRC_INFO, Defs::Log::ElementRenderSummary, solid_throws, " solids removed, ", solid_renders, " solid renders, ", translucent_renders, " translucent renders, ", shader_switches, " shader switches, ", texture_switches, " texture switches" );
}

}
