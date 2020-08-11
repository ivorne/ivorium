#include "Camera.hpp"

#include "../Defs.hpp"
#include "ElementSystem.hpp"

namespace iv
{

Camera::Camera( Instance * inst, RenderTarget::Geometry geometry ) :
    cm( inst, this, "Camera", ClientMarker::Status() ),
    Slot( inst ),
    ElementRenderer( inst ),
    viewport_size()
{
    this->cm.inherits( this->Slot::cm, this->ElementRenderer::cm );
    
    this->geometry( geometry );
    
    auto es = this->instance()->getSystem< ElementSystem >();
    if( es )
        es->camera_created( this );
}

Camera::~Camera()
{
    auto es = this->instance()->getSystem< ElementSystem >();
    if( es )
        es->camera_destroyed( this );
}

void Camera::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Camera" );
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "projection", this->state.projection );
    row.Column( "pixelizing_projection", this->state.pixelizing_projection );
    row.Column( "view", this->state.view );
}

void Camera::geometry( RenderTarget::Geometry geometry )
{
    this->viewport_size = geometry.size;
    
    float3 new_size( geometry.size.x / geometry.density, geometry.size.y / geometry.density, 1000 );
    this->expectedSize.Set( new_size );
    this->size.Set( new_size );
    
    this->state.projection = glm::scale( float4x4( 1 ), float3( 1.0f, -1.0f, 1.0f ) ) // Y inversion
                                * glm::ortho( 0.0f, geometry.size.x / geometry.density, 0.0f, geometry.size.y / geometry.density, 1.0f, -1001.0f );
                                
    this->state.pixelizing_projection = glm::scale( float4x4( 1 ), float3( 1.0f, -1.0f, 1.0f ) ) // Y inversion
                                * glm::ortho( 0.0f, float( geometry.size.x ), 0.0f, float( geometry.size.y ), -1.0f, 1.0f );
}

void Camera::render_scene( Elem * view )
{
    if( view )
        this->state.view = glm::inverse( view->modelTransform.Get() );
    else
        this->state.view = iv::float4x4( 1 );
    
    this->init_frame();
    this->cm.log( SRC_INFO, Defs::Log::ElementFirstPass, "Start first pass." );
    this->first_pass( this );
    this->RunSecondPasses();
    this->cm.log( SRC_INFO, Defs::Log::ElementRenderPass, "Start render pass." );
    this->RunRender( state );
    this->end_frame();
}

Camera * Camera::elem_getRoot()
{
    return this;
}

//==================================================================================================
//--------------------- math -----------------------------------------------------------------------
float3 Camera::WNormalized( float4 in )
{
    if( in.w == 0.0f )
        return float3( in.x, in.y, in.z );
    else
        return float3( in.x / in.w, in.y / in.w, in.z / in.w );
}

float3 Camera::FromViewportSpaceToCameraSpace( float3 screen_point )
{
    float2 view_size = this->viewport_size;
    
    screen_point.y = view_size.y - screen_point.y;  // Flip Y coordinate, because natively 3D Y coordinate begins at bottom but we have it on top (to match gui construction).
    
    float3 camera_point;
    camera_point.x = screen_point.x / view_size.x * 2.0f - 1.0f;
    camera_point.y = screen_point.y / view_size.y * 2.0f - 1.0f;
    camera_point.z = screen_point.z * 2.0f - 1.0f;
    
    return camera_point;
}

float3 Camera::FromCameraSpaceToViewportSpace( float3 camera_point )
{
    float2 view_size = this->viewport_size;
    
    float3 screen_point;
    screen_point.x = ( camera_point.x + 1.0f ) * 0.5f * view_size.x;
    screen_point.y = ( camera_point.y + 1.0f ) * 0.5f * view_size.y;
    screen_point.y = view_size.y - screen_point.y;  // Flip Y coordinate, because natively 3D Y coordinate begins at bottom but we have it on top (to match gui construction).
    screen_point.z = ( camera_point.z + 1.0f ) * 0.5f;
    
    return screen_point;
}

float3 Camera::FromLocalSpaceToViewportSpace( float4x4 model_transform, float3 in )
{
    float4x4 mvp = this->state.projection * this->state.view * model_transform;
    
    float4 camera_space = mvp * float4( in.x, in.y, in.z, 1.0 );
    return this->FromCameraSpaceToViewportSpace( this->WNormalized( camera_space ) );
}

float3 Camera::FromViewportSpaceToLocalSpace( float4x4 model_transform, float3 in )
{
    float4x4 mvp = this->state.projection * this->state.view * model_transform;
    
    float3 camera_space = FromViewportSpaceToCameraSpace( in );
    float4 local_space = glm::inverse( mvp ) * float4( camera_space.x, camera_space.y, camera_space.z, 1.0f );
    return this->WNormalized( local_space );
}

float2 Camera::FromViewportPlaneToLocalPlane( float4x4 model_transform, float2 in )
{
    float4x4 mvp = this->state.projection * this->state.view * model_transform;
    float4x4 mvp_inv = glm::inverse( mvp );
    
    float4 local_lorigin( 0, 0, 0, 1 ); // local origin seen by localy-relative observer
    float3 camera_lorigin = this->WNormalized( mvp * local_lorigin ); // local origin seen by camera-relative observer
    
    
    float3 camera_in = this->FromViewportSpaceToCameraSpace( float3( in.x, in.y, 0.0 ) ); // input point seen by camera-relative observer
    float3 camera_move( camera_in.x - camera_lorigin.x, camera_in.y - camera_lorigin.y, 0.0 ); // distance between input point and local origin seen by camera-relative observer
    
    // adjust Z
    camera_move.z = ( -1.0f * mvp_inv[ 0 ][ 2 ] * camera_move.x - mvp_inv[ 1 ][ 2 ] * camera_move.y ) / mvp_inv[ 2 ][ 2 ];
    camera_in.z = camera_lorigin.z + camera_move.z;
    
    //
    float4 local_in = mvp_inv * float4( camera_in.x, camera_in.y, camera_in.z, 1.0f ); // input point seen by localy-relative observer
    float3 out = this->WNormalized( local_in );
    return float2( out.x, out.y );
}

}
