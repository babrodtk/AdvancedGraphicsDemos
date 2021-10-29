#ifndef _RAYTRACER_H__
#define _RAYTRACER_H__

#include <memory>
#include <string>
#include <vector>

#include "FrameBuffer.hpp"
#include "SceneObject.hpp"
#include "RayTracerState.hpp"

class RayTracer {
public:
	RayTracer(unsigned int width, unsigned int height);

	/**
	  * Adds an object to the scene
	  */
	void addSceneObject(std::shared_ptr<SceneObject>& o);

	/**
	  * Renders the current scene
	  */
	void render();

	/**
	  * Saves the currently rendered frame as an image file
	  */
	void save(std::string basename, std::string extension);

private:
	std::shared_ptr<FrameBuffer> fb;
	std::shared_ptr<RayTracerState> state;

	glm::vec2 multisample[4];

	/**
	  * Defines the virtual screen we project our rays through
	  */
	struct {
		float left;
		float right;
		float top;
		float bottom;
	} screen;
};

#endif