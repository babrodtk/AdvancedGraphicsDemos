#version 150

uniform sampler2D my_texture;
uniform float dy;
out vec4 out_color;
smooth in vec2 texCoord;

void main() {
	float Y;
	float min_exp = 0.55;
	const float weights[4] = float[](0.383f, 0.242f, 0.061f, 0.006f);

	vec3 color = weights[0] * texture2D(my_texture, texCoord).rgb;
	for (int i=1; i<3; ++i) {
		color += weights[i] * texture2D(my_texture, vec2(texCoord.x, texCoord.y-i*dy)).rgb;
		color += weights[i] * texture2D(my_texture, vec2(texCoord.x, texCoord.y+i*dy)).rgb;
	}

	Y = dot(vec3(0.30, 0.59, 0.11), color);
	Y = (float(Y > min_exp) * (Y-min_exp))*4.0;

	//color = Y*color;

	out_color = vec4(color, 1.0);
}