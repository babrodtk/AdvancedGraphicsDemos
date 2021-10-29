#version 150
 
uniform mat4 P; //Projection matrix
uniform mat4 MV; //Modelview matrix

layout(triangles) in;
layout(triangle_strip, max_vertices = 30) out;

flat in vec3 g_c[3];   // Color
smooth in vec3 g_v[3]; // View vector
smooth in vec3 g_l[3]; // Light vector
smooth in vec3 g_n[3]; // Normal

flat out vec3 f_c;   // Color
smooth out vec3 f_v; // View vector
smooth out vec3 f_l; // Light vector
smooth out vec3 f_n; // Normal
smooth out vec3 f_d; // "Triangle distance"

vec4 barycentric(in vec4 p_i, in vec4 p_j, in vec4 p_k, in float u, in float v) {
	float w = 1.0 - u - v;
	return u*p_i + v*p_j + w*p_k;
}

vec3 barycentric(in vec3 p_i, in vec3 p_j, in vec3 p_k, in float u, in float v) {
	float w = 1.0 - u - v;
	return u*p_i + v*p_j + w*p_k;
}

vec4 pi(in vec4 q, in vec4 p_i, in vec3 n_i) {
	vec4 n = vec4(n_i, 0.0);
	return q - (dot(q - p_i, n)) * n;
}

void phong_interp(in vec4 p_i, in vec4 p_j, in vec4 p_k, 
				in vec3 n_i, in vec3 n_j, in vec3 n_k, 
				in vec3 v_i, in vec3 v_j, in vec3 v_k, 
				in vec3 l_i, in vec3 l_j, in vec3 l_k, 
				in vec3 c_i, in vec3 c_j, in vec3 c_k, 
				in float u, in float v) {
	//First, set the output variables for this u-v position
	f_n = barycentric(n_i, n_j, n_k, u, v);
	f_v = barycentric(v_i, v_j, v_k, u, v);
	f_l = barycentric(l_i, l_j, l_k, u, v);
	f_c = barycentric(c_i, c_j, c_k, u, v);

	//Find coordinate of our point from the barycentric u-v coordinates
	vec4 p = barycentric(p_i, p_j, p_k, u, v);
	
	f_d = barycentric(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), u, v);

	//Find projection onto the planes
	vec4 p1 = pi(p, p_i, n_i);
	vec4 p2 = pi(p, p_j, n_j);
	vec4 p3 = pi(p, p_k, n_k);
	
	//Return the barycentric interpolation of the projected points
	gl_Position = P * MV * barycentric(p1, p2, p3, u, v);
	EmitVertex();
}

void main() {
	vec4 p_i = gl_in[0].gl_Position;
	vec4 p_j = gl_in[1].gl_Position;
	vec4 p_k = gl_in[2].gl_Position;

	vec3 n_i = g_n[0];
	vec3 n_j = g_n[1];
	vec3 n_k = g_n[2];
	
	vec3 v_i = g_v[0];
	vec3 v_j = g_v[1];
	vec3 v_k = g_v[2];
	
	vec3 l_i = g_l[0];
	vec3 l_j = g_l[1];
	vec3 l_k = g_l[2];
	
	vec3 c_i = g_c[0];
	vec3 c_j = g_c[1];
	vec3 c_k = g_c[2];

	/**
	  * This subdivides a triangle as follows:
	  *
	  *       p_k
	  *       /\
	  *      /  \  <- top
	  *     /____\
	  *    /\    /\
	  *   /  \  /  \  <- base
	  *  /____\/____\
	  * p_i         p_j
	  *
	  */
	//First triangle strip (base, right to left)
	phong_interp(p_i, p_j, p_k, n_i, n_j, n_k, v_i, v_j, v_k, l_i, l_j, l_k, c_i, c_j, c_k, 0.0, 1.0); 
	phong_interp(p_i, p_j, p_k, n_i, n_j, n_k, v_i, v_j, v_k, l_i, l_j, l_k, c_i, c_j, c_k, 0.0, 0.5); 
	phong_interp(p_i, p_j, p_k, n_i, n_j, n_k, v_i, v_j, v_k, l_i, l_j, l_k, c_i, c_j, c_k, 0.5, 0.5); 
	phong_interp(p_i, p_j, p_k, n_i, n_j, n_k, v_i, v_j, v_k, l_i, l_j, l_k, c_i, c_j, c_k, 0.5, 0.0); 
	phong_interp(p_i, p_j, p_k, n_i, n_j, n_k, v_i, v_j, v_k, l_i, l_j, l_k, c_i, c_j, c_k, 1.0, 0.0); 
	EndPrimitive();

	//Second triangle strip (top)
	phong_interp(p_i, p_j, p_k, n_i, n_j, n_k, v_i, v_j, v_k, l_i, l_j, l_k, c_i, c_j, c_k, 0.0, 0.5); 
	phong_interp(p_i, p_j, p_k, n_i, n_j, n_k, v_i, v_j, v_k, l_i, l_j, l_k, c_i, c_j, c_k, 0.0, 0.0); 
	phong_interp(p_i, p_j, p_k, n_i, n_j, n_k, v_i, v_j, v_k, l_i, l_j, l_k, c_i, c_j, c_k, 0.5, 0.0); 
	EndPrimitive();
	
	/*
	for(int i = 0; i < gl_in.length(); i++) {
		f_n = g_n[i];
		f_v = g_v[i];
		f_l = g_l[i];
		f_c = g_c[i];
		
		gl_Position =   P * MV * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
	*/
}