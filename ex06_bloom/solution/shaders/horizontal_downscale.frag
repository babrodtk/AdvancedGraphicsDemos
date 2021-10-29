#version 150

uniform sampler2D my_texture;
uniform float dx;
out vec4 out_color;
smooth in vec2 texCoord;

void main() {
	const float weights[4] = float[](0.383, 0.242, 0.061, 0.006);
	
	vec3 color = weights[0] * texture2D(my_texture, texCoord).rgb;
	for (int i=1; i<4; ++i) {
		color += weights[i] * texture2D(my_texture, vec2(texCoord.x-i*dx, texCoord.y)).rgb;
		color += weights[i] * texture2D(my_texture, vec2(texCoord.x+i*dx, texCoord.y)).rgb;
	}

	out_color = vec4(color, 1.0);
}