
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D mask;
uniform sampler2D grass;
uniform sampler2D rocks;
uniform float u_time;

void main()
{
	vec2 uv = v_uv;
	//per fer interpolacio de textures
	vec4 grass = texture2D(grass,uv*10);
	vec4 piedra = texture2D(rocks, uv*10);
	float mask = texture2D(mask, uv).x;
	vec4 color = grass*mask + piedra*(1-mask);
	gl_FragColor = color;
}
