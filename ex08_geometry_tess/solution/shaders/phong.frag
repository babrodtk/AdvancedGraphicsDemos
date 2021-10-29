#version 150

flat in vec3 f_c;
smooth in vec3 f_n;
smooth in vec3 f_v;
smooth in vec3 f_l;
smooth in vec3 f_d;

out vec4 out_color;

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

void main() {
	float d1 = min(min(f_d.x, f_d.y), f_d.z);

    vec3 h = normalize(f_v+f_l);
    vec3 n = normalize(f_n);
    float diff = max(0.0f, dot(n, f_l));
    float spec = pow(max(0.0f, dot(n, h)), 128.0f);
	vec3 color = amplify(d1, 40, -0.5) * (diff*f_c + spec);

    out_color = vec4(color, 1.0);
}