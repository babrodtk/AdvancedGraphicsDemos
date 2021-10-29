#version 150

smooth in vec2 coord;
out vec4 out_color;
uniform float iterations;
uniform float zoom;

vec4 HSV2RGB(vec4 hsva) { 
  vec4 rgba; 
  float h = hsva.x, s = hsva.y, v = hsva.z, m, n, f; 
  float i;   
  if( h == 0.0 ) 
    rgba = vec4(v, v, v, hsva.a); 
  else { 
    i = floor(h); 
    f = h - i; 
    float t = i / 2.0; 
    if( t - floor( t ) <  0.1 ) 
      f = 1.0 - f; // if i is even 
    m = v * (1.0 - s); 
    n = v * (1.0 - s * f); 
    if(i == 0.0 )       rgba = vec4(v, n, m, hsva.a); 
    else if( i == 1.0 ) rgba = vec4(n, v, m, hsva.a); 
    else if( i == 2.0 ) rgba = vec4(m, v, n, hsva.a); 
    else if( i == 3.0 ) rgba = vec4(m, n, v, hsva.a); 
    else if( i == 4.0 ) rgba = vec4(n, m, v, hsva.a); 
    else                rgba = vec4(v, m, n, hsva.a); 
  }
  return rgba; 
}

void main() {
	vec2 z0 = coord;
	vec2 z = z0;
	int i = 0;
	float t;
	
	const vec4 hsv1 = vec4(0, 1, 1, 1);
	const vec4 hsv2 = vec4(6.28318531, 1, 1, 1);

	while (z.x*z.x + z.y*z.y < 25.0 && i < iterations) {
		float tmp = z.x*z.x - z.y*z.y + z0.x;
		z.y = 2*z.x*z.y + z0.y;
		z.x = tmp;
		++i;
	}
	
	if (i < iterations) {
		t = mod((i - log(log(length(z))/log(5.0))/log(2.0)) / 100, 1.0);
		//t = log(log(length(z))/log(5.0))/log(2.0);
		//t = (i - log(log(length(z))/log(5.0))/log(2.0)) / iterations;
		vec4 hsv = (1-t) * hsv1 + t* hsv2;
		out_color = HSV2RGB(hsv);
	}
	else {
		out_color = vec4(0.0f);
	}
}