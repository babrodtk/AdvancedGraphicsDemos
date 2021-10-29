#version 150

flat in vec3 f_c;
smooth in vec3 f_n;
smooth in vec3 f_v;
smooth in vec3 f_l;

out vec4 out_color;


void main() {
    vec3 h = normalize(f_v+f_l);
    vec3 n = normalize(f_n);
    float diff = max(0.0f, dot(n, f_l));
    float spec = pow(max(0.0f, dot(n, h)), 128.0f);
	vec3 color = diff*f_c + spec;

    out_color = vec4(color, 1.0);
}