#version 150

in vec2 v_pos;

uniform vec2 position;
uniform float zoom;

smooth out vec2 coord;

void main() {
	gl_Position = vec4(v_pos, 0.0, 1.0);
	coord = position + v_pos * zoom;
}