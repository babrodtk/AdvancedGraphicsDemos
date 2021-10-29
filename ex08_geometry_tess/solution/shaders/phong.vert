#version 150

//Input uniforms
uniform mat4 IMV; //Inverse modelview

//Input variables
in  vec3 v_p; //Vertex shader position
in  vec3 v_n; //Vertex shader normal

//Ourput variables to geometry shader
flat out vec3 g_c;   // Color
smooth out vec3 g_v; // View vector
smooth out vec3 g_l; // Light vector
smooth out vec3 g_n; // Normal

void main() {
	vec4 position = vec4(v_p, 1.0);
	gl_Position = position;
	
	float h_d = (1.0f/IMV[3].w);
	g_v = normalize(IMV[3].xyz*h_d - position.xyz);
	g_l = normalize(vec3(IMV * vec4(200.0f, 200.0f, 200.0f, 1.0))*h_d - position.xyz);
	g_c = vec3(0.5f, 0.7f, 0.5f);
	g_n = normalize(v_n);
}