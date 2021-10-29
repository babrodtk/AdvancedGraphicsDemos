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
	
	//Initialize state
	state.reset(new RayTracerState(camera_position));
	
	//Initialize IL and ILU
	ilInit();
	iluInit();

	srand(time(NULL));
}

void RayTracer::addSceneObject(std::shared_ptr<SceneObject>& o) {
	state->getScene().push_back(o);
}

void RayTracer::render() {
	//For every pixel, ray-trace using multiple CPUs
#ifdef _OPENMP
#pragma omp parallel for
	for (int j=0; j<static_cast<int>(fb->getHeight()); ++j) {
#else
	for (unsigned int j=0; j<fb->getHeight(); ++j) {
#endif
		for (unsigned int i=0; i<fb->getWidth(); ++i) {
			glm::vec3 out_color(0.0, 0.0, 0.0);
			float t = std::numeric_limits<float>::max();
			int rays = 1000;
			float x, y, z;

			// "Flipped" ray definition Create the ray using the view screen definition	
			x = 0.5*(screen.right - i*(screen.right-screen.left)/static_cast<float>(fb->getWidth()));
			y = 0.5*(screen.top - j*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()));
			z = 1.0f;
			
			//Find the virtual screen position at the z=1 plane
			glm::vec3 virtual_screen_pixel_pos = glm::vec3(x, y, z);

			//Set the center of the apperture at the z=0.5 plane
			glm::vec3 aperture_center = glm::vec3(0, 0, 0.5);

			//Find the direction of the pinhole ray
			glm::vec3 pinhole_direction = (aperture_center - virtual_screen_pixel_pos) * 2.0f;

			//Find the focus point
			glm::vec3 focus_point = 7.0f * pinhole_direction;

			//Create a lot of random rays
			for (int k=0; k<rays; ++k) {
				const float rad = 0.125;
				//Generate a random point that is within the apperture
				float x_rand = 2 * rad * rand() / static_cast<float>(RAND_MAX) - rad;
				float y_rand = 2 * rad * rand() / static_cast<float>(RAND_MAX) - rad;
				while (x_rand * x_rand + y_rand*y_rand > rad*rad) {
					x_rand = 2 * rad * rand() / static_cast<float>(RAND_MAX) - rad;
					y_rand = 2 * rad * rand() / static_cast<float>(RAND_MAX) - rad;
				}
				
				//Find the offset from the apperture center to the random point
				glm::vec3 random_vec = glm::vec3(x_rand, y_rand, 0.0f);

				//Find the random point on the apperture
				glm::vec3 aperture_random_point = aperture_center + random_vec;
				
				//Find the vector from the pixel position to the random apperture point
				glm::vec3 random_direction = (aperture_random_point - virtual_screen_pixel_pos);
				
				//Find the random position on the virtual lens
				glm::vec3 virtual_lens_ray_pos = virtual_screen_pixel_pos + random_direction*2.0f;
				
				//Find the ray from the random position on the virtual lens to the focus point
				glm::vec3 direction = focus_point - virtual_lens_ray_pos;

				//Create ray
				Ray r = Ray(state->getCamPos() + virtual_lens_ray_pos, direction);

				//Add the color from the ray-tracing to the pixel color
				out_color += state->rayTrace(r);
			}
			//Find average ray color
			out_color /= static_cast<float>(rays);

			//Set output color
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

	//Find a unique filename...
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