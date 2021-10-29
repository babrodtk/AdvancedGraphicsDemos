#version 130

in vec3 ex_Color;
in vec3 ex_Normal;
in vec3 ex_View;
in vec3 ex_Light;
out vec4 res_Color;

void main() {
    vec3 v = normalize(ex_View);
    vec3 l = normalize(ex_Light);
    vec3 n = normalize(ex_Normal);
    vec3 h = normalize(v+l);
    float diff = max(0.0f, dot(l, n));
    float spec = pow(max(0.0f, dot(h, n)), 128.0f);
    res_Color = diff*vec4(ex_Color, 1.0f) + vec4(spec);
};