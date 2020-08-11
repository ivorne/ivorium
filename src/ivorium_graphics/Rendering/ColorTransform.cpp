#include "ColorTransform.hpp"

#include <ivorium_core/ivorium_core.hpp>
#include <cmath>

namespace iv
{

float4x4 ColorTransform::RotateHue( float radians )
{
    return glm::translate( float4x4( 1 ), float3( radians, 0, 0 ) );
}

float4x4 ColorTransform::ShiftSaturation( float shift )
{
    return glm::translate( float4x4( 1 ), float3( 0, shift, 0 ) );
}

float4x4 ColorTransform::ScaleValue( float scale )
{
    return glm::scale( float4x4( 1 ), float3( 1, 1, scale ) );
}

float4x4 ColorTransform::ShiftValue( float shift )
{
    return glm::translate( float4x4( 1 ), float3( 0, 0, shift ) );
}

float4x4 ColorTransform::Change_Hue( float4 from, float4 to )
{
    float hfrom = ColorTransform::GetHue( from );
    float hto = ColorTransform::GetHue( to );
    return ColorTransform::RotateHue( hto - hfrom );
}

float4x4 ColorTransform::Change_Saturation( float4 from, float4 to )
{
    float cfrom = ColorTransform::GetSaturation( from );
    float cto = ColorTransform::GetSaturation( to );
    return ColorTransform::ShiftSaturation( cto - cfrom );
}

float4x4 ColorTransform::Change_Value( float4 from, float4 to )
{
    float lfrom = ColorTransform::GetValue( from );
    float lto = ColorTransform::GetValue( to );
    return ColorTransform::ScaleValue( lto / lfrom );
}

float4x4 ColorTransform::Change( float4 from, float4 to )
{
    float4 from_hsv = linearRGB_to_HSV( sRGB_to_linearRGB( from ) );
    float4 to_hsv = linearRGB_to_HSV( sRGB_to_linearRGB( to ) );
    return ColorTransform::RotateHue( to_hsv.x - from_hsv.x ) * ColorTransform::ShiftSaturation( to_hsv.y - from_hsv.y ) * ColorTransform::ScaleValue( to_hsv.z / from_hsv.z );
}

float4x4 ColorTransform::Zero()
{
    return glm::scale( float4x4( 1 ), float3( 0, 0, 0 ) );
}

float4x4 ColorTransform::Scale( float4 scale )
{
    float4 val = ColorTransform::linearRGB_to_HSV( ColorTransform::sRGB_to_linearRGB( scale ) );
    return glm::scale( float4x4( 1 ), float3( val.x, val.y, val.z ) );
}

float4x4 ColorTransform::Shift( float4 shift )
{
    float4 val = ColorTransform::linearRGB_to_HSV( ColorTransform::sRGB_to_linearRGB( shift ) );
    return glm::translate( float4x4( 1 ), float3( val.x, val.y, val.z ) );
}

float ColorTransform::GetHue( float4 color )
{
    return ColorTransform::linearRGB_to_HSV( ColorTransform::sRGB_to_linearRGB( color ) ).x;
}

float ColorTransform::GetSaturation( float4 color )
{
    return ColorTransform::linearRGB_to_HSV( ColorTransform::sRGB_to_linearRGB( color ) ).y;
}

float ColorTransform::GetValue( float4 color )
{
    return ColorTransform::linearRGB_to_HSV( ColorTransform::sRGB_to_linearRGB( color ) ).z;
}

float4 ColorTransform::Apply( float4x4 transform, float4 color )
{
    float4 incolor_hsv = linearRGB_to_HSV( sRGB_to_linearRGB( color ) );
    float4 outcolor_hsv = transform * incolor_hsv;
    return linearRGB_to_sRGB( HSV_to_linearRGB( outcolor_hsv ) );
}

float ColorTransform::sRGB_to_linearRGB( float val )
{
    if( val <= 0.0404482362771082f )
        return val / 12.92f;
    else
        return std::pow( ( val + 0.055f ) / 1.055f, 2.4f );
}

float4 ColorTransform::sRGB_to_linearRGB( float4 val )
{
    return float4( sRGB_to_linearRGB( val.x ), sRGB_to_linearRGB( val.y ), sRGB_to_linearRGB( val.z ), val.w );
}

float ColorTransform::linearRGB_to_sRGB( float val )
{
    if( val <= 0.00313066844250063f )
        return val * 12.92f;
    else
        return 1.055f * std::pow( val, 1.0f / 2.4f ) - 0.055f;
}

float4 ColorTransform::linearRGB_to_sRGB( float4 val )
{
    return float4( linearRGB_to_sRGB( val.x ), linearRGB_to_sRGB( val.y ), linearRGB_to_sRGB( val.z ), val.w );
}

float4 ColorTransform::linearRGB_to_HSV( float4 rgb )
{
    float R = rgb.x;
    float G = rgb.y;
    float B = rgb.z;
    
    float Min = std::min( R, std::min( G, B ) );
    float Max = std::max( R, std::max( G, B ) );
    
    float PI = 3.1415926535f;
    float V = Max;
    float C = Max - Min;
    float S;
    if( V == 0.0f )
        S = 0.0f;
    else
        S = C / V;
    
    float H;
    
    if( C == 0.0f )
        H = 0.0f;
    else if( V == R )
        H = PI / 3.0f * ( 0.0f + ( G - B ) / C );
    else if( V == G )
        H = PI / 3.0f * ( 2.0f + ( B - R ) / C );
    else
        H = PI / 3.0f * ( 4.0f + ( R - G ) / C );
    
    return float4( H, S, V, rgb.w );
}

float4 ColorTransform::HSV_to_linearRGB( float4 hsv )
{
    float H = hsv.x;
    float S = std::clamp( hsv.y, 0.0f, 1.0f );
    float V = std::clamp( hsv.z, 0.0f, 1.0f );
    
    float PI = 3.1415926535f;
    float C = V * S;
    float Hn = sig_fmod( H, 2.0f * PI ) / ( PI / 3.0f );
    float X = C * ( 1.0f - std::abs( std::fmod( Hn, 2.0f ) - 1.0f ) );
    
    float m = V - C;
    float R = m;
    float G = m;
    float B = m;
    
    if( Hn <= 1.0f )
    {
        R += C;
        G += X;
    }
    else if( Hn <= 2.0f )
    {
        G += C;
        R += X;
    }
    else if( Hn <= 3.0f )
    {
        G += C;
        B += X;
    }
    else if( Hn <= 4.0f )
    {
        B += C;
        G += X;
    }
    else if( Hn <= 5.0f )
    {
        B += C;
        R += X;
    }
    else if( Hn <= 6.0f )
    {
        R += C;
        B += X;
    }
    
    return float4( R, G, B, hsv.w );
}

}
