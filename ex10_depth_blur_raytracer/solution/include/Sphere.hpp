#ifndef _SPHERE_HPP__
#define _SPHERE_HPP__

#include "RayTracerState.hpp"
#include "SceneObject.hpp"
#include "SceneObjectEffect.hpp"

#include <glm/glm.hpp>

/**
  * The sphere is a scene object that is easy to work with. We have
  * simple analytical formulations for both the intersection with a ray and
  * for computation of the normal at any point
  */
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
	float intersect(const Ray& r);
	
	/**
	  * Computes normal for an intersection point on a sphere
	  */
	const glm::vec3 computeNormal(const Ray& r, const float& t);

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		glm::vec3 normal = computeNormal(ray, t);
		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p; //< center of sphere
	float r;   //< sphere radius
};

#endif