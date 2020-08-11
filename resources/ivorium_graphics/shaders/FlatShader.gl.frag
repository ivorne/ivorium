#version 300 es

precision mediump float;

//------------------- uniforms ----------------
// space transforms
uniform mat4 		local_to_pixel_space;
uniform mat4 		pixel_to_local_space;

// mesh
uniform vec3 		mesh_resize;
uniform vec2 		mesh_texcoord_density;

// texture
uniform sampler2D 	tex0;
uniform vec2 		tex0_size;			//< Physical size of texture.
uniform float 		tex0_density_rel;
uniform int			tex0_color_space;

// translucency
uniform bool 		translucent;	//< If this is false, then all pixels are either fully visible (alpha = 1.0) or they are thrown away.
uniform vec4 		preblend;

// fitting stage
uniform int 		fitting_stage;

// scissor stage
uniform bool 		scissor_enabled;
uniform mat4 		local_to_scissor_space;
uniform vec3 		scissor_size;

// pixelize stage
uniform int 		pixelize_stage;
uniform vec2 		pixelize_size;
uniform vec2 		pixelize_offset;

// resize stage
uniform int 		resize_stage;
uniform vec2 		resize_anchor;

// filtering stage
uniform int 		filtering_stage;
uniform float 		filtering_msdf_pixel_range;
uniform float 		filtering_alpha_threshold;
uniform float 		filtering_alpha_width;

// color stage
uniform mat4 		color_transform;

// alpha stage
uniform float 		alpha;

// color space in which the framebuffer accepts data from shader
uniform int 		framebuffer_color_space;

//------------- consts -----------------
const int PixelizeStage_None     = 0;
const int PixelizeStage_Squares  = 1;
const int PixelizeStage_Circles  = 2;

const int ResizeStage_Scale  = 0;
const int ResizeStage_Fixed  = 1;
const int ResizeStage_Repeat = 2;
const int ResizeStage_Frame  = 3;

const int FilteringStage_Plain 						= 0;
const int FilteringStage_Msdf  						= 1;
const int FilteringStage_AlphaThreshold    			= 2;
const int FilteringStage_AlphaThresholdWidth		= 3;
const int FilteringStage_AlphaThresholdWidthSmooth	= 4;


const int ColorSpace_sRGB   = 0;
const int ColorSpace_Linear = 1;
//------------- in ---------------
in vec2 frag_texcoord;
in vec3 frag_position;

//----------- out ----------------------
out vec4 outputColor;

//------------ common functions --------------------------
float median( float r, float g, float b )
{
    return max(min(r, g), min(max(r, g), b));
}

float signed_fmod( float val, float mod )
{
	if( ( val < 0.0f ) == ( mod < 0.0f ) )
		return val - mod * float( int( val / mod ) );
	else
		return val - mod * float( int( val / mod ) - 1 );
}

vec2 signed_fmod( vec2 value, vec2 mod )
{
	return vec2( signed_fmod( value.x, mod.x ), signed_fmod( value.y, mod.y ) );
}

vec3 w_normalize( vec4 val )
{
	if( val.w == 0.0f )
		return val.xyz;
	
	return vec3( val.x / val.w, val.y / val.w, val.z / val.w );
}

float Linear_to_sRGB_1( float val )
{
	if( val <= 0.00313066844250063f )
		return val * 12.92f;
	else
		return 1.055f * pow( val, 1.0f / 2.4f ) - 0.055f;
}

vec4 Linear_to_sRGB( vec4 val )
{
	return vec4( Linear_to_sRGB_1( val.x ), Linear_to_sRGB_1( val.y ), Linear_to_sRGB_1( val.z ), val.w );
}

float sRGB_to_Linear_1( float val )
{
	if( val <= 0.0404482362771082f )
		return val / 12.92f;
	else
		return pow( ( val + 0.055f ) / 1.055f, 2.4f );
}

vec4 sRGB_to_Linear( vec4 val )
{
	return vec4( sRGB_to_Linear_1( val.x ), sRGB_to_Linear_1( val.y ), sRGB_to_Linear_1( val.z ), val.w );
}

vec4 linearRGB_to_HSV( vec4 rgb )
{
	float R = rgb.x;
	float G = rgb.y;
	float B = rgb.z;
	
	float PI = 3.1415926535f;
	float Min = min( R, min( G, B ) );
	float Max = max( R, max( G, B ) );
	
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
	
	return vec4( H, S, V, rgb.w );
}

vec4 HSV_to_linearRGB( vec4 hsv )
{
	float H = hsv.x;
	float S = clamp( hsv.y, 0.0f, 1.0f );
	float V = clamp( hsv.z, 0.0f, 1.0f );
	
	float PI = 3.1415926535f;
	float C = V * S;
	float Hn = signed_fmod( H, 2.0f * PI ) / ( PI / 3.0f );
	float X = C * ( 1.0f - abs( mod( Hn, 2.0f ) - 1.0f ) );
	
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
	
	return vec4( R, G, B, hsv.w );
}

//------------------------------------------------------------------------------------------------
//------------ scissor stage ---------------------------------------------------------------------
void run_scissor_stage( in vec2 texcoord, in vec3 position, out bool discarded )
{
	discarded = false;
	
	if( !scissor_enabled )
		return;
	
	vec3 mvpos = w_normalize( local_to_scissor_space * vec4( position, 1.0f ) );
	
	if( mvpos.x < 0.0f || mvpos.y < 0.0f || mvpos.z < 0.0f || mvpos.x > scissor_size.x || mvpos.y > scissor_size.y || mvpos.z > scissor_size.z )
	{
		discarded = true;
		return;
	}
}

//------------------------------------------------------------------------------------------------
//------------ pixelize stage --------------------------------------------------------------------
void run_pixelize_stage( in vec2 texcoord, out vec2 texcoord_pixelized, out bool discarded )
{
	discarded = false;
	
	if( pixelize_stage == PixelizeStage_None )
	{
		texcoord_pixelized = texcoord;
	}
	else if( pixelize_stage == PixelizeStage_Squares )
	{
		// local space pixelization
		if( false )
		{
			vec2 local_px = ( texcoord - pixelize_offset ) * mesh_resize.xy / mesh_texcoord_density * tex0_density_rel;
			vec2 local_px_rounded = floor( local_px / pixelize_size ) * pixelize_size;
			texcoord_pixelized = local_px_rounded * mesh_texcoord_density / tex0_density_rel / mesh_resize.xy + pixelize_offset;
		}
		
		// screen space pixelization
		if( true )
		{
			vec2 local_px = ( texcoord - pixelize_offset ) * mesh_resize.xy / mesh_texcoord_density;
			
			vec3 pixelspace_px = w_normalize( local_to_pixel_space * vec4( local_px, 0.0f, 1.0f ) );
			vec3 pixelspace_px_rounded = vec3( floor( pixelspace_px.xy / pixelize_size ) * pixelize_size, pixelspace_px.z );
			vec3 local_px_rounded = w_normalize( pixel_to_local_space * vec4( pixelspace_px_rounded, 1.0f ) );
			
			texcoord_pixelized = local_px_rounded.xy * mesh_texcoord_density / mesh_resize.xy + pixelize_offset;
		}
	}
	else if( pixelize_stage == PixelizeStage_Circles )
	{
		// screen space pixelization
		vec2 local_px = ( texcoord - pixelize_offset ) * mesh_resize.xy / mesh_texcoord_density;
		
		vec3 pixelspace_px = w_normalize( local_to_pixel_space * vec4( local_px, 0.0f, 1.0f ) );
		vec3 pixelspace_px_rounded = vec3( floor( pixelspace_px.xy / pixelize_size ) * pixelize_size, pixelspace_px.z );
		vec3 local_px_rounded = w_normalize( pixel_to_local_space * vec4( pixelspace_px_rounded, 1.0f ) );
		
		float center_dist = length( ( pixelspace_px_rounded.xy + pixelize_size / 2.0f ) - ( pixelspace_px.xy ) );
		if( center_dist > ( pixelize_size.x + pixelize_size.y ) / 4.0 )	// divided by 2 for averaging, divided by 2 to transform diameter to radius
		{
			discarded = true;
			return;
		}
		
		texcoord_pixelized = local_px_rounded.xy * mesh_texcoord_density / mesh_resize.xy + pixelize_offset;
	}
}

//--------------------------------------------------
//------------ resize stage ------------------------
float texcoord_frame_transform( float coord_image_px, float tex_size, float image_size )
{
	float coord_tex_px;
	if( coord_image_px < image_size * 0.5 )
	{// left half
		if( coord_image_px < tex_size / 2.0f )
			coord_tex_px = coord_image_px;
		else
			coord_tex_px = tex_size / 2.0f;
	}
	else
	{// right half
		if( image_size - coord_image_px < tex_size / 2.0f )
			coord_tex_px = tex_size - ( image_size - coord_image_px );
		else
			coord_tex_px = tex_size / 2.0f;
	}
	
	return coord_tex_px;
}

// texture_scale is used for example in msdf filtering for precise antialiasing
void run_resize_stage( in vec2 texcoord, out vec2 resized_texcoord, out float texel_density, out bool discarded )
{
	discarded = false;
	
	if( resize_stage == ResizeStage_Scale )
	{
		resized_texcoord = texcoord;
		
		float avg_mesh_resize = ( mesh_resize.x + mesh_resize.y ) / 2.0f;
		vec2 mesh_texel_density = mesh_texcoord_density * tex0_size;
		float avg_texel_density = ( mesh_texel_density.x + mesh_texel_density.y ) / 2.0f;
		texel_density = avg_texel_density / avg_mesh_resize;
	}
	else if( resize_stage == ResizeStage_Fixed )
	{
		vec2 resized_texcoord_px = ( texcoord - resize_anchor ) * mesh_resize.xy / mesh_texcoord_density * tex0_density_rel;
		resized_texcoord = resized_texcoord_px / tex0_size + resize_anchor;
		texel_density = 1.0f / tex0_density_rel;
		
		if( resized_texcoord.x < 0.0f || resized_texcoord.x > 1.0f || resized_texcoord.y < 0.0f || resized_texcoord.y > 1.0f )
		{
			discarded = true;
			return;
		}
	}
	else if( resize_stage == ResizeStage_Repeat )
	{
		vec2 resized_texcoord_px = ( texcoord - resize_anchor ) * mesh_resize.xy / mesh_texcoord_density * tex0_density_rel;
		resized_texcoord = mod( resized_texcoord_px / tex0_size + resize_anchor, vec2( 1.0f, 1.0f ) );
		texel_density = 1.0f / tex0_density_rel;
	}
	else if( resize_stage == ResizeStage_Frame )
	{
		vec2 image_size = mesh_resize.xy / mesh_texcoord_density * tex0_density_rel;
		vec2 resized_texcoord_px = texcoord * image_size;
		
		vec2 coord_tex_px;
		coord_tex_px.x = texcoord_frame_transform( resized_texcoord_px.x, tex0_size.x, image_size.x );
		coord_tex_px.y = texcoord_frame_transform( resized_texcoord_px.y, tex0_size.y, image_size.y );
		resized_texcoord = coord_tex_px / tex0_size;
		texel_density = 1.0f / tex0_density_rel;
	}
}

//--------------------------------------------------
//------------ filtering stage ---------------------
void run_filtering_stage( in vec3 position, in vec2 resized_texcoord, in float texel_density, out vec4 texcolor, out bool discarded )
{
	discarded = false;
	
	vec4 texel = texture( tex0, resized_texcoord );
	if( tex0_color_space == ColorSpace_sRGB )
		texel = sRGB_to_Linear( texel );
	
	if( filtering_stage == FilteringStage_Plain )
	{
		texcolor = texel;
	}
	else if( filtering_stage == FilteringStage_Msdf )
	{
		if( translucent || preblend.a > 0.01f )
		{
			float opacity;
			
			// pixel space transform method
			if( false )
			{
				// texture space to local space scale
				float texture_to_local_scale = 1.0f / texel_density;
				
				// local space to pixel space scale
				vec3 pixelspace_position = w_normalize( local_to_pixel_space * vec4( position, 1.0f ) );
				vec3 offseted_pixelspace_position = pixelspace_position + vec3( 1.0f, 1.0f, 0.0f );
				vec3 offseted_position = w_normalize( pixel_to_local_space * vec4( offseted_pixelspace_position, 1.0f ) );
				float pixel_to_local_scale = length( offseted_position - position ) / 1.42f;

				// texture to pixel space scale
				float texture_to_pixel_scale = texture_to_local_scale / pixel_to_local_scale;
				
				// msdf
				vec3 smpl = texel.rgb;
				float sigDist = median( smpl.r, smpl.g, smpl.b );
				
				// Looks better when working in linear space but framebuffer is in sRGB.
				float threshold = 0.50f;
				float smoothing = 1.0f / texture_to_pixel_scale / filtering_msdf_pixel_range / 2.0f;
				opacity = smoothstep( max( 0.0f, 0.5f - smoothing ), min( 1.0f, 0.5f + smoothing ), sigDist + 0.5f - threshold );
				
				// Looks good when staying in sRGB color space.
				//float smoothing = 1.0f / texture_to_pixel_scale / filtering_msdf_pixel_range / 1.8f;
				//opacity = clamp( ( sigDist - max( 0.0f, 0.5f - smoothing ) ) / ( min( 1.0f, 0.5f + smoothing ) - max( 0.0f, 0.5f - smoothing ) ), 0.0f, 1.0f );
			}
			
			
			// fwidth method
			if( true )
			{
				vec3 smpl = texel.rgb;
				float sigDist = median( smpl.r, smpl.g, smpl.b );
				vec2 texel = resized_texcoord * tex0_size;
				
				// Looks better when working in linear space but framebuffer is in sRGB.
				float threshold = 0.50f;
				float smoothing = length( fwidth( texel ) ) / 2.0f / filtering_msdf_pixel_range;		// optimal for diagonal: 1.5f, optimal for orthogonal (maybe both?): 2.0f
				opacity = smoothstep( max( 0.0f, 0.5f - smoothing ), min( 1.0f, 0.5f + smoothing ), sigDist + 0.5f - threshold );
				//opacity = sigDist;
				
				// Looks good when staying in sRGB color space.
				//float smoothing = length( fwidth( texel ) ) / 1.8f / filtering_msdf_pixel_range;		// optimal for diagonal: 1.5f, optimal for orthogonal (maybe both?): 2.0f
				//opacity = clamp( ( sigDist - max( 0.0f, 0.5f - smoothing ) ) / ( min( 1.0f, 0.5f + smoothing ) - max( 0.0f, 0.5f - smoothing ) ), 0.0f, 1.0f );
			}
			
			//
			texcolor = vec4( 1.0f, 1.0f, 1.0f, opacity );
		}
		else
		{
			// msdf
			vec3 smpl = texel.rgb;
			float sigDist = median( smpl.r, smpl.g, smpl.b );
			float opacity = sigDist > 0.5f ? 1.0f : 0.0f;
			
			//
			texcolor = vec4( 1.0f, 1.0f, 1.0f, 0.0f );
		}
	}
	else if( filtering_stage == FilteringStage_AlphaThreshold )
	{
		vec4 smpl = texel;
		
		if( smpl.a <= 0.0f )
		{
			discarded = true;
			return;
		}
		
		if( filtering_alpha_threshold <= ( 1.0f - smpl.a ) )
		{
			discarded = true;
			return;
		}
		
		texcolor = vec4( smpl.rgb, 1.0f );
		return;
	}
	else if( filtering_stage == FilteringStage_AlphaThresholdWidth )
	{
		vec4 smpl = texel;
		
		if( smpl.a <= 0.0f )
		{
			discarded = true;
			return;
		}
		
		if( filtering_alpha_threshold <= ( 1.0f - smpl.a ) )
		{
			discarded = true;
			return;
		}
		
		if( filtering_alpha_threshold > filtering_alpha_width + ( 1.0f - smpl.a ) )
		{
			discarded = true;
			return;
		}
		
		texcolor = vec4( smpl.rgb, 1.0f );
	}
	else if( filtering_stage == FilteringStage_AlphaThresholdWidthSmooth )
	{
		vec4 smpl = texel;
		
		if( smpl.a <= 0.0f )
		{
			discarded = true;
			return;
		}
		
		if( filtering_alpha_threshold <= ( 1.0f - smpl.a ) )
		{
			discarded = true;
			return;
		}
		
		if( filtering_alpha_threshold > filtering_alpha_width + ( 1.0f - smpl.a ) )
		{
			discarded = true;
			return;
		}
		
		float smoothed = smoothstep( filtering_alpha_threshold, filtering_alpha_threshold - filtering_alpha_width, 1.0f - smpl.a );
		float symmetrical = ( smoothed < 0.5f )  ?  ( smoothed * 2.0f ) :  ( 1.0f - ( smoothed - 0.5 ) * 2.0f );
		texcolor = vec4( smpl.rgb, symmetrical );
	}
}

//--------------------------------------------------
//------------ color stage -------------------------
vec3 transform_color( vec3 vec, mat4 mat )
{
	// transform
	vec3 col = w_normalize( mat * vec4( vec, 1.0f ) );
	
	//
	return col;
}

void run_color_stage( in vec4 incolor, out vec4 outcolor )
{
	// color transform
	vec4 incolor_hsv = linearRGB_to_HSV( incolor );
	vec4 outcolor_hsv = vec4( transform_color( incolor_hsv.rgb, color_transform ), incolor.a * alpha );
	outcolor = HSV_to_linearRGB( outcolor_hsv );
	
	// preblend
	if( outcolor.a > 1.0f / 300.0f && preblend.a > 1.0f / 300.0f )
	{
		outcolor.r = outcolor.a * outcolor.r + ( 1.0f - outcolor.a ) * preblend.r * preblend.a;
		outcolor.g = outcolor.a * outcolor.g + ( 1.0f - outcolor.a ) * preblend.g * preblend.a;
		outcolor.b = outcolor.a * outcolor.b + ( 1.0f - outcolor.a ) * preblend.b * preblend.a;
		outcolor.a = preblend.a * 1.0f + ( 1.0f - preblend.a ) * outcolor.a;
		outcolor.r /= outcolor.a;
		outcolor.g /= outcolor.a;
		outcolor.b /= outcolor.a;
	}
}

//--------------------------------------------------
//------------ alpha stage -------------------------
void run_alpha_stage( in vec4 incolor, out vec4 outcolor, out bool discarded )
{
	discarded = false;
	
	if( translucent )
	{
		outcolor = vec4( incolor.rgb, incolor.a );
	}
	else
	{
		if( incolor.a < 0.5f )
		{
			discarded = true;
			return;
		}
		
		outcolor = vec4( incolor.rgb, 1.0f );
	}
}

//--------------------------------------------------
void main()
{
	bool discarded;
	
	// fitting stage
	if( fitting_stage == -1 )
	{
		outputColor = vec4( 1, 0, 0, 1 );	// Just to prevent variable fitting_stage from being optimized out.
		return;
	}
	
	// scissor stage (can discard some fragments, usualy based on frag_pos
	run_scissor_stage( frag_texcoord, frag_position, discarded );
	if( discarded )
		discard;
	
	// pixelize stage (runs effects on image-relative coordinates)
	vec2 texcoord_pixelized;
	run_pixelize_stage( frag_texcoord, texcoord_pixelized, discarded );
	if( discarded )
		discard;
	
	// resize stage (transforms image-relative coordinates to texture coordinates)
	vec2 texcoord_pixelized_resized;
	float texel_density;	// scale from texture space to local space
	run_resize_stage( texcoord_pixelized, texcoord_pixelized_resized, texel_density, discarded );
	if( discarded )
		discard;
	
	// filtering stage (retrieves color from texture sampler on given texture coordinate)
	vec4 texcolor;
	run_filtering_stage( frag_position, texcoord_pixelized_resized, texel_density, texcolor, discarded );
	if( discarded )
		discard;
	
	// color stage (runs effects on texture color)
	vec4 outcolor;
	run_color_stage( texcolor, outcolor );
	
	// alpha stage
	vec4 alpha_outcolor;
	run_alpha_stage( outcolor, alpha_outcolor, discarded );
	if( discarded )
		discard;
	
	// color space
	if( framebuffer_color_space == ColorSpace_sRGB )
		alpha_outcolor = Linear_to_sRGB( alpha_outcolor );
	
	//
	outputColor = alpha_outcolor;
}
