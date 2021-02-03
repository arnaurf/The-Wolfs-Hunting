
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform vec3 camera_pos;


uniform float max_fog;
uniform float min_fog;
void main()
{
	vec2 uv = v_uv;
	
	
	float dist = length( camera_pos - v_world_position );
	float fog_factor = clamp( 1.0 - (max_fog - dist) / (max_fog - min_fog), 0.0,1.0);
	
	vec4 text = texture2D( u_texture, uv);
	if(text.a < 0.5)
		discard;
		
	gl_FragColor = mix( u_color, vec4(0.89,0.89,0.89,1), fog_factor);
}
