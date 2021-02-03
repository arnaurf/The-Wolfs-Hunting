
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D colormap;
uniform sampler2D detail;
uniform vec3 camera_pos;

uniform float max_fog;
uniform float min_fog;

void main()
{
	vec2 uv = v_uv;
	
	float dist = length( camera_pos - v_world_position );
	float fog_factor = clamp( 1.0 - (max_fog - dist) / (max_fog - min_fog), 0.0,1.0);
	
	//per fer interpolacio de textures
	vec4 color = texture2D(colormap,uv);
	vec4 detalle1;
	vec4 detalle2;
	vec4 detalle3;
	detalle1 = texture2D(detail,uv*80);
	detalle2 = texture2D(detail,uv*40);
	detalle3 = texture2D(detail,uv*15);
		
	float coef1 = clamp(dist/180, 0, 1);
	float coef2 = clamp( (dist-180)/100, 0, 1);
	
	
	
	gl_FragColor = mix( color + (detalle2*coef1*(1-coef2) + detalle3*coef2 + detalle1*(1-coef1) - vec4(0.5)), vec4(0.89,0.89,0.89,1), fog_factor);
}
