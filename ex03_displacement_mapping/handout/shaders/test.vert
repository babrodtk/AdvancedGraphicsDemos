#version 130

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normal;

in  vec2 in_Position;
out vec3 ex_Color;

void main() {
   vec4 pos = vec4(in_Position.x, 1.0, in_Position.y, 1.0);
	gl_Position = projection * view * model * pos;
	ex_Color = vec3(0.5f, 0.5f, 1.0f);
};
