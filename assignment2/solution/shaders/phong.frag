#version 150

uniform vec3 color;
uniform sampler2DShadow shadowmap;
uniform samplerCube cubemap;
smooth in vec3 f_n;
smooth in vec3 f_v;
smooth in vec3 f_l;
smooth in vec4 f_light_texcoord;
out vec4 out_color;

void main() {
	vec3 l = normalize(f_l);
    vec3 h = normalize(normalize(f_v)+l);
    vec3 n = normalize(f_n);
	
    float diff = max(0.0f, dot(n, l));
    float spec = pow(max(0.0f, dot(n, h)), 128.0f);
	
	float tmp = 0.0f;
	if (f_light_texcoord.x > 0.0 && f_light_texcoord.x < f_light_texcoord.w && f_light_texcoord.y > 0.0 && f_light_texcoord.y < f_light_texcoord.w)
		tmp = float((textureProj(shadowmap, f_light_texcoord) - f_light_texcoord.z/f_light_texcoord.w) > 0);
	
    out_color = vec4(0.3*texture(cubemap, n).x*color, 1.0) + vec4((0.5+0.5*tmp)*(diff*color + tmp*spec), 1.0);
}