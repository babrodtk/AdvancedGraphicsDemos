#version 150

uniform vec4 light_pos;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
 
void main() {
	float q = 0.1;
	vec4 l;

	if (gl_in[0].gl_Position.x==0.0) {
		l = light_pos;
	}
	else {
		l = light_pos;
	}
	
	vec4 a = l + q*vec4( 1.0, -1.0, 0.0, 0.0);
	vec4 b = l + q*vec4( 1.0,  1.0, 0.0, 0.0);
	vec4 c = l + q*vec4(-1.0, -1.0, 0.0, 0.0);
	vec4 d = l + q*vec4(-1.0,  1.0, 0.0, 0.0);
	
	gl_Position =  a; EmitVertex();
	gl_Position =  b; EmitVertex();
	gl_Position =  c; EmitVertex();
	gl_Position =  d; EmitVertex();
	EndPrimitive();
}