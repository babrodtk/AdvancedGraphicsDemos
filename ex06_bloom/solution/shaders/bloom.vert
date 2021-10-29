#version 150

in vec2 position;
smooth out vec2 texCoord;
uniform sampler2D my_texture;
flat out float exposure;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
	texCoord = position * 0.5 + 0.5;
	exposure = dot(vec3(0.30, 0.59, 0.11), textureLod(my_texture, texCoord, 99).rgb);
}