#version 300 es

precision mediump float;

//------------------- uniforms ----------------
// space transforms
uniform mat4 projection_view_model;
uniform mat4 local_to_pixel_space;
uniform mat4 pixel_to_local_space;
uniform bool depth_override_enabled;
uniform float depth_override;

// mesh
uniform vec3 mesh_resize;

// fitting stage
//uniform int fitting_stage;

//---------- in ------------
in vec3 position;
in vec2 texcoord;

//---------- out -----------
out vec3 frag_position;
out vec2 frag_texcoord;

//---------- code ---------------
void main()
{
	// texcoord
	frag_position = position * mesh_resize;
	frag_texcoord = texcoord;
	
	// TODO - pixel fitting
	
	// vertex coord
	vec4 pos = projection_view_model * vec4( frag_position, 1.0f );
	if( depth_override_enabled )
		pos.z = depth_override;
	
	gl_Position = pos;
}
