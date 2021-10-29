#ifndef SCENEOBJECTEFFECT_HPP__
#define SCENEOBJECTEFFECT_HPP__

#include "Ray.hpp"
#include "RayTracerState.hpp"

class SceneObjectEffect {
public:
	virtual glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) = 0;
private:
};

class PhongEffect : public SceneObjectEffect {
public:
	PhongEffect(glm::vec3 pos=glm::vec3(0.0),
				glm::vec3 diff=glm::vec3(0.5),
				glm::vec3 spec=glm::vec3(0.5)) {
		this->pos = pos;
		this->diff = diff;
		this->spec = spec;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		glm::vec3 p = ray.getOrigin() + t*ray.getDirection();
		glm::vec3 l = glm::normalize(pos - p);
		glm::vec3 v = glm::normalize(-ray.getDirection());
		glm::vec3 h = glm::normalize(l+v);
		
		glm::vec3 out_color = glm::vec3(0.0);
		out_color += std::max(glm::dot(normal, l), 0.0f)*diff;
		out_color += std::pow(glm::dot(normal, h), 128.0f)*spec;

		return out_color;
	}
private:
	glm::vec3 pos;
	glm::vec3 diff;
	glm::vec3 spec;
};

class SteelEffect : public SceneObjectEffect {
public:
	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		glm::vec3 r = glm::reflect(ray.getDirection(), normal);
		Ray subray = ray.spawn(t, r);

		return state.rayTrace(subray)*(0.5f+0.5f*std::powf(glm::dot(normal, glm::normalize(-ray.getDirection())), 0.2));
	}
};


class FresnelEffect : public SceneObjectEffect {
public:
	FresnelEffect(glm::vec3 color=glm::vec3(1.0f, 1.0f, 1.0f)) {
		this->color = color;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		const float eta_air = 1.000293f;
		const float eta_carbondioxide = 1.00045f;
		const float eta_water = 1.3330f;
		const float eta_ethanol = 1.361f;
		const float eta_pyrex = 1.470f;
		const float eta_window_glass = 1.515f;
		const float eta_diamond = 2.419f;

		const float eta0 = eta_air;
		const float eta1 = eta_window_glass;

		float eta, R0;
		float d = 1.0f;
		int k_min;
		glm::vec3 out_color(0.0f);
 
		glm::vec3 v = glm::normalize(-ray.getDirection());
		glm::vec3 r1, r2;
		float fresnel;

		if (glm::dot(v, normal) >= 0.0f) {
			eta = eta0/eta1;
			R0 = std::powf((eta0-eta1)/(eta0+eta1), 2.0f);

			r1 = glm::reflect(-v, normal);
			r2 = glm::refract(-v, normal, eta);
			fresnel = R0 + (1.0f-R0)*powf((1.0f-glm::dot(v, normal)), 5.0f);
		}
		else {
			eta = eta1/eta0;
			R0 = std::powf((eta1-eta0)/(eta1+eta0), 2.0f);

			r1 = glm::reflect(-v, -normal);
			r2 = glm::refract(-v, -normal, eta);
			fresnel = R0 + (1.0f-R0)*powf((1.0f-glm::dot(v, -normal)), 5.0f);
			
			float c = 50.0f;
			d = std::max((c-glm::length(t*ray.getDirection())) / c, 0.0f);
			d = d*d;
		}
		
		//Reflection
		Ray subray1 = ray.spawn(t, r1);
		glm::vec3 reflect_color = state.rayTrace(subray1);
		
		//Refraction
		Ray subray2 = ray.spawn(t, r2);
		glm::vec3 refract_color = state.rayTrace(subray2);

		return fresnel*reflect_color + d*color*(1.0f-fresnel)*refract_color;
	}

private:
	glm::vec3 color;
};

#endif