#version 150

smooth in vec3 tex_coord;
out vec4 out_color;

void main() {
	//FIXME: Create a texture sample for the cube map, and
	//use the texture coordinate to fetch a texel
	vec3 c = abs(tex_coord);
	if (c.x > c.y && c.x > c.z)
		out_color = vec4(1.0, 0.0, 0.0, 1.0);
	if (c.y > c.x && c.y > c.z)
		out_color = vec4(0.0, 1.0, 0.0, 1.0);
	if (c.z > c.x && c.z > c.y)
		out_color = vec4(0.0, 0.0, 1.0, 1.0);
}