#version 150

smooth in vec3 pass_n;
smooth in vec3 pass_v;

out vec4 out_color;

void main() {
	const float eta_air = 1.000293;
	const float eta_carbondioxide = 1.00045;
	const float eta_water = 1.3330;
	const float eta_ethanol = 1.361;
	const float eta_pyrex = 1.470;
	const float eta_diamond = 2.419;

	const float eta0 = eta_air;
	const float eta1 = eta_pyrex;

	const float eta = eta0/eta1;
	const float R0 = 0; //FIXME: Implement R0

	vec3 n = normalize(pass_n);
	vec3 v = normalize(pass_v);

	//FIXME: Implement lookup into the cube map and fresnel rendering

	out_color = vec4(vec3(dot(n, v)), 1.0);
}