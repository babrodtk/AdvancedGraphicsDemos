#version 150
 
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

smooth in vec3 g_n[3];
smooth in vec3 g_v[3];
smooth in vec3 g_l[3];
smooth in vec4 light_texcoord[3];

smooth out vec3 f_n;
smooth out vec3 f_v;
smooth out vec3 f_l;
smooth out vec4 f_light_texcoord;
 
void main() {
	vec4 cog = vec4(0.0);
	for(int i = 0; i < gl_in.length(); i++) {
		cog += gl_in[i].gl_Position;
	}
	cog /= 3.0;
	
	for(int i = 0; i < gl_in.length(); i++) {
		f_n = g_n[i];
		f_v = g_v[i];
		f_l = g_l[i];
		f_light_texcoord = light_texcoord[i];
		
		vec4 v0 = normalize(cog-gl_in[i].gl_Position);
		gl_Position =  gl_in[i].gl_Position;// + 0.01*v0;
		EmitVertex();
	}
	EndPrimitive();
	/*
	for(int i = 0; i < gl_in.length(); i++) {
		f_n = g_n[i];
		f_v = g_v[i];
		f_l = g_l[i];
		f_c = vec3(0.0);
		
		gl_Position =  gl_in[i].gl_Position + vec4(0, 0, 0.1 , 0);
		EmitVertex();
	}
	EndPrimitive();
	*/
}