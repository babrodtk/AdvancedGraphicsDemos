#include "RayTracer.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <sys/stat.h>

#include <IL/il.h>
#include <IL/ilu.h>

#include "CubeMap.hpp"

RayTracer::RayTracer(unsigned int width, unsigned int height) {
	const glm::vec3 camera_position(0.0f, 0.0f, 10.0f);

	//Initialize framebuffer and virtual screen
	fb.reset(new FrameBuffer(width, height));
	float aspect = width/static_cast<float>(height);
	screen.top = 1.0f;
	screen.bottom = -1.0f;
	screen.right = aspect;
	screen.left = -aspect;

	//Initialize multisample locations
	multisample[0] = glm::vec2(-0.25f, -0.25f);
	multisample[1] = glm::vec2(+0.25f, -0.25f);
	multisample[2] = glm::vec2(-0.25f, +0.25f);
	multisample[3] = glm::vec2(+0.25f, +0.25f);
	
	//Initialize state
	state.reset(new RayTracerState(camera_position));
	
	//Initialize IL and ILU
	ilInit();
	iluInit();

	//Initialize randomness
	srand(time(NULL));
}

void RayTracer::addSceneObject(std::shared_ptr<SceneObject>& o) {
	state->getScene().push_back(o);
}

void RayTracer::render() {
	//For every pixel
#pragma omp parallel for
#ifdef _OPENMP
	for (int j=0; j<static_cast<int>(fb->getHeight()); ++j) {
#else
	for (unsigned int j=0; j<fb->getHeight(); ++j) {
#endif
		for (unsigned int i=0; i<fb->getWidth(); ++i) {
			glm::vec3 out_color(0.0, 0.0, 0.0);

			for (int k=0; k<4; ++k) {
				float t = std::numeric_limits<float>::max();
				float x, y, z;

				// Create the ray using the view screen definition and multisample location
				x = i+multisample[k].x;
				y = j+multisample[k].y;

				x = x*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
				y = y*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
				z = -1.0f;

				glm::vec3 origin = state->getCamPos() + glm::vec3(x, y, 0);
				glm::vec3 focus_point = origin + glm::vec3(x, y, z)*10.0f;

#if 0
				const unsigned int n_rays = 100;

				for (int i=0; i<n_rays; ++i) {
					float theta = 6.28318531 * rand() / static_cast<float>(RAND_MAX);
					float r = 0.2 * rand() / static_cast<float>(RAND_MAX);
					float x_rand = r*cos(theta);
					float y_rand = r*sin(theta);
					glm::vec3 r_origin = (origin + glm::vec3(x_rand, y_rand, 0.0f));
					glm::vec3 r_direction = focus_point - r_origin;
					Ray ray = Ray(r_origin, r_direction);
					
					//Now do the ray-tracing to shade the pixel
					out_color += 0.25f*state->rayTrace(ray)/static_cast<float>(n_rays);
				}
#else
				Ray ray = Ray(origin, glm::vec3(x, y, z));
				out_color += 0.25f*state->rayTrace(ray);
#endif
			}

			fb->setPixel(i, j, out_color);
		}
		std::cout << "Line " << j << " done (" << 100*j/static_cast<float>(fb->getHeight()) << ")%" << std::endl;
	}
}

void RayTracer::save(std::string basename, std::string extension) {
	ILuint texid;
	struct stat buffer;
	int i;
	std::stringstream filename;

	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	//Create image
	ilGenImages(1, &texid);
	ilBindImage(texid);
	//FIXME: Ugly const cast:( DevILs fault, unfortunately
	ilTexImage(fb->getWidth(), fb->getHeight(), 1, 3, IL_RGB, IL_FLOAT, const_cast<float*>(fb->getData().data()));

	//Find an unique filename...
	for (i=0; i<10000; ++i) {
		filename.str("");
		filename << basename << std::setw(4) << std::setfill('0') << i << "." << extension;
		if (stat(filename.str().c_str(), &buffer) != 0) break;
	}

	if (i == 10000) {
		std::stringstream log;
		log << "Unable to find unique filename for " << basename << "%d." << extension;
		throw std::runtime_error(log.str());
	}

	if (!ilSaveImage(filename.str().c_str())) {
		std::stringstream log;
		log << "Unable to save " << filename.str();
		throw std::runtime_error(log.str());
	}
	else {
		std::cout << "Saved " << filename.str() << std::endl;
	}

	ilDeleteImages(1, &texid);
}