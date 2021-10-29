#version 150
uniform mat4 transform;

in vec3 position;

void main() {
	gl_Position = transform * vec4(position, 1.0);
}