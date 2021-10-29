#version 150
uniform mat4 modelviewprojection_matrix;
uniform mat4 modelview_matrix_inverse;
uniform mat4 light_matrix;
uniform vec3 light_pos;

in vec3 position;
in vec3 normal;

smooth out vec3 g_v;
smooth out vec3 g_l;
smooth out vec3 g_n;
smooth out vec4 light_texcoord;

void main() {	
	float homogeneous_divide = (1.0f/modelview_matrix_inverse[3].w);
	vec3 cam_pos_world = modelview_matrix_inverse[3].xyz*homogeneous_divide;

	g_v = normalize(cam_pos_world - position);
	g_l = normalize(light_pos - position);
	g_n = normalize(normal);
		
	mat4 tmp = mat4(0.5, 0, 0, 0.0,
		0, 0.5, 0, 0.0,
		0, 0, 0.5*0.99, 0.0,
		0.5, 0.5, 0.5, 1);
	light_texcoord = tmp*light_matrix*vec4(position, 1.0);

	gl_Position = modelviewprojection_matrix * vec4(position, 1.0);;
}