#ifndef _SPHERE_HPP__
#define _SPHERE_HPP__

#include "RayTracerState.hpp"
#include "SceneObject.hpp"
#include "SceneObjectEffect.hpp"

#include <glm/glm.hpp>

class Sphere : public SceneObject {
public:
	Sphere(glm::vec3 center, float radius, std::shared_ptr<SceneObjectEffect> effect) {
		this->p = center;
		this->r = radius;
		this->effect = effect;
	}

	/**
	  * Computes the ray-sphere intersection
	  */
	float intersect(const Ray& r) {
		const float z_offset = 10e-4f;
		const glm::vec3 d = r.getDirection();
		const glm::vec3 p0 = r.getOrigin();
		float a = glm::dot(d, d);
		float b = 2.0f*glm::dot(d, (p0-p));
		float c = glm::dot(p0-p, p0-p)-this->r*this->r;
		
		//Solve ax^2 + bx + c = 0
		if ((b*b-4.0f*a*c) < 0.0f) {
			//No intersections
			return -1;
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
	
	const glm::vec3 computeNormal(const Ray& r, const float& t) {
		return (r.getOrigin() + t*r.getDirection() - p) / this->r;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		glm::vec3 normal = computeNormal(ray, t);
		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p; //< center of sphere
	float r;   //< sphere radius
};

#endif