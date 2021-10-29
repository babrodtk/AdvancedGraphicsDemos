#include "SceneObjectEffect.hpp"
#include "Sphere.hpp"


glm::vec3 PhongEffect::rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
	glm::vec3 p = ray.getOrigin() + t*ray.getDirection();
	glm::vec3 l = glm::normalize(light_pos - p);
	glm::vec3 v = glm::normalize(-ray.getDirection());
	glm::vec3 h = glm::normalize(l+v);
		
	glm::vec3 out_color = glm::vec3(0.0);
	out_color += std::max(glm::dot(normal, l), 0.0f)*light_diff;
	out_color += std::pow(glm::dot(normal, h), 128.0f)*light_spec;

	return out_color;
}

float Sphere::intersect(const Ray& r) {
	const float z_offset = 10e-4f;
	const glm::vec3 d = r.getDirection();
	const glm::vec3 p0 = r.getOrigin();
	float a = glm::dot(d, d);
	float b = 2.0f*glm::dot(d, (p0-p));
	float c = glm::dot(p0-p, p0-p)-this->r*this->r;
		
	//Solve ax^2 + bx + c = 0
	if ((b*b-4.0f*a*c) < 0.0f) {
		//No intersections
		return false;
	}
	else {
		//One or more intersections
		float w1 = sqrt(b*b - 4.0f*a*c);
		float w2 = 1.0f/(2.0f*a);

		float t0, t1, tmp;
		t0 = (-b + w1)*w2;
		tmp = (-b - w1)*w2;
		t1 = std::max(t0, tmp); //t1 is the "largest" intersection
		t0 = std::min(t0, tmp); //t0 is the "smallest" intersection

		if (t0 > z_offset) 
			return t0;
		else if (t1 > z_offset)
			return t1;
		else
			return -1;
	}
}

const glm::vec3 Sphere::computeNormal(const Ray& r, const float& t) {
	return (r.getOrigin() + t*r.getDirection() - p) / this->r;
}