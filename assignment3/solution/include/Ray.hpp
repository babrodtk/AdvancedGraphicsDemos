#ifndef _RAY_HPP__
#define _RAY_HPP__

#include <glm/glm.hpp>

class Ray {
public:
	Ray(glm::vec3 origin, glm::vec3 direction) {
		this->origin = origin;
		this->direction = direction;
		depth = 0;
	}

	/**
	  * Returns the origin of the ray
	  */
	inline const glm::vec3& getOrigin() const { return origin; }

	/**
	  * Returns the direction of the ray
	  */
	inline const glm::vec3& getDirection() const { return direction; }

	inline Ray spawn(float t, glm::vec3 d) const {
		Ray r(getOrigin()+t*getDirection(), d);
		r.depth = this->depth + 1;
		return r;
	}

	inline bool isValid() const {
		return (depth <= max_depth);
	}

	inline void invalidate() {
		this->depth = max_depth;
	}

private:
	friend class RayTracer;

	static const unsigned int max_depth = 7;
	unsigned int depth;
	glm::vec3 origin;
	glm::vec3 direction;
};

#endif