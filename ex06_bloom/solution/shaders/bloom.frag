#version 150

uniform sampler2D my_texture;
uniform sampler2D my_bloom_texture;
out vec4 out_color;
smooth in vec2 texCoord;
flat in float exposure;

void main() {
	vec4 color;
	vec4 bloom;
	const float brightMax = 5.0;

	bloom = texture2D(my_bloom_texture, texCoord);
	color = texture2D(my_texture, texCoord);
	float YD = exposure * (exposure/brightMax + 1.0) / (exposure + 1.0);

	if (texCoord.x < 0.5) {
		out_color = (color + bloom)*YD;
	}
	else {
		out_color = color+bloom;
	}
	//out_color = 0.000001*color + bloom;
	//out_color = color + 0.000001*bloom;
	//out_color = color;
	//out_color = vec4(texture2D(my_texture, texCoord).rgb, 1.0);
}