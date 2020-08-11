#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    Input colors are in sRGB.
    Matrices are to be applied in HSV space.
*/
struct ColorTransform
{
    // Matrix manipulation
    static float4x4         RotateHue( float radians );
    static float4x4         ShiftSaturation( float );
    static float4x4         ScaleValue( float );
    
    static float4x4         ShiftValue( float );
    
    static float4x4         Change_Hue( float4 from, float4 to );
    static float4x4         Change_Saturation( float4 from, float4 to );
    static float4x4         Change_Value( float4 from, float4 to );
    
    static float4x4         Change( float4 from, float4 to );
    
    static float4x4         Zero();
    static float4x4         Scale( float4 sRGB );   ///< Input value is in sRGB, it is transformed to HCL and matrix is scaled by the HCL value.
    static float4x4         Shift( float4 sRGB );
    
    // Color manipulation
    static float            GetHue( float4 color );
    static float            GetSaturation( float4 color );      ///< Returned values are strange (they do not stay fixed when changing hue), but are consistent with the rest of the model.
    static float            GetValue( float4 color );   ///< May also behave a bit strange (similarly to GetSaturation) but to significantly smaller degree.
    
    static float4           Apply( float4x4 transform, float4 color );  ///< Transform generated from funcitons in ColorTransform, color in sRGB, returns color in sRGB with transform applied to it. Do not use simple matrix multiplication, that has to be done fully in linear space.
    
    // Helpers.
    static float            sRGB_to_linearRGB( float );
    static float4           sRGB_to_linearRGB( float4 );
    
    static float            linearRGB_to_sRGB( float );
    static float4           linearRGB_to_sRGB( float4 );
    
    static float4           linearRGB_to_HSV( float4 );
    static float4           HSV_to_linearRGB( float4 );
};

}
