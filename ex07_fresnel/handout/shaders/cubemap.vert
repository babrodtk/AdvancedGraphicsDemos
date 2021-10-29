#version 150

uniform mat4 transform;

in vec3 position;
smooth out vec3 tex_coord;

void main() {
	vec4 pos = transform * vec4(position*2, 1.0);
	tex_coord = position.xyz;
	gl_Position = pos;
}