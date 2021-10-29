#ifndef _CUBEMAP_HPP__
#define _CUBEMAP_HPP__

#include <string>
#include <limits>
#include <sstream>

#include <glm/glm.hpp>

#include <IL/il.h>
#include <IL/ilu.h>

class CubeMap : public SceneObject {
public:
	CubeMap(std::string posx, std::string negx, 
			std::string posy, std::string negy,
			std::string posz, std::string negz) {
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		loadImage(posx, this->posx);
		loadImage(negx, this->negx);
		loadImage(posy, this->posy);
		loadImage(negy, this->negy);
		loadImage(posz, this->posz);
		loadImage(negz, this->negz);
	}
	
	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		glm::vec3 out_color;
		glm::vec3 dir =  ray.getDirection();

		//x is major axis
		if (fabs(dir.x) >= fabs(dir.y) && fabs(dir.x) >= fabs(dir.z)) {
			float s = 0.5f-0.5f*dir.z/dir.x;
			float t = 0.5f-0.5f*dir.y/dir.x;
			if (dir.x > 0)
				out_color = readTexture(posx, s, t);
			else
				out_color = readTexture(negx, s, 1.0f-t);
		}
		//y is major axis
		if (fabs(dir.y) >= fabs(dir.x) && fabs(dir.y) >= fabs(dir.z)) {
			float s = 0.5f*dir.x/dir.y+0.5f;
			float t = 0.5f*dir.z/dir.y+0.5f;
			if (dir.y > 0)
				out_color = readTexture(posy, s, t);
			else
				out_color = readTexture(negy, 1.0f-s, t);
		}
		//z is major axis
		else if(fabs(dir.z) >= fabs(dir.x) && fabs(dir.z) >= fabs(dir.y)) {
			float s = 0.5f*dir.x/dir.z+0.5f;
			float t = 0.5f*dir.y/dir.z+0.5f;
			if (dir.z > 0)
				out_color = readTexture(posz, s, 1.0f-t);
			else
				out_color = readTexture(negz, s, t);
		}

		ray.invalidate();
		return out_color;
	}
	
	float intersect(const Ray& r) {
		return std::numeric_limits<float>::max();
	}

private:
	struct texture {
		std::vector<float> data;
		unsigned int width;
		unsigned int height;
	};

	static glm::vec3 readTexture(texture& tex, float s, float t) {
		glm::vec3 out_color;

		float xf = std::min(s*tex.width, tex.width-1.0f);
		float yf = std::min(t*tex.height, tex.height-1.0f);

		unsigned int xm = static_cast<unsigned int>(xf);
		unsigned int ym = static_cast<unsigned int>(yf);
		unsigned int xp = std::min(xm+1, tex.width-1);
		unsigned int yp = std::min(ym+1, tex.height-1);


		float xs = xf - xm;
		float ys = yf - ym;
		
		unsigned int i0 = (ym*tex.width + xm)*3;
		unsigned int i1 = (ym*tex.width + xp)*3;
		unsigned int i2 = (yp*tex.width + xm)*3;
		unsigned int i3 = (yp*tex.width + xp)*3;

		for (int k=0; k<3; ++k) {
			float c0 = tex.data.at(i0+k);
			float c1 = tex.data.at(i1+k);
			float c2 = tex.data.at(i2+k);
			float c3 = tex.data.at(i3+k);
			
			float d0 = (1.0f-xs)*c0 + xs*c1;
			float d1 = (1.0f-xs)*c2 + xs*c3;

			out_color[k] = (1.0f-ys)*d0 + ys*d1;
		}

		return out_color;
	}

	static void loadImage(std::string filename, texture& tex) {
		ILuint ImageName;

		ilGenImages(1, &ImageName); // Grab a new image name.
		ilBindImage(ImageName); 
		
		if (!ilLoadImage(filename.c_str())) {
			ILenum e;
			std::stringstream error;
			while ((e = ilGetError()) != IL_NO_ERROR) {
				error << e << ": " << iluErrorString(e) << std::endl;
			}
			ilDeleteImages(1, &ImageName); // Delete the image name. 
			throw std::runtime_error(error.str());
		}

		tex.width = ilGetInteger(IL_IMAGE_WIDTH); // getting image width
		tex.height = ilGetInteger(IL_IMAGE_HEIGHT); // and height
		tex.data.resize(tex.width*tex.height*3);
		
		ilCopyPixels(0, 0, 0, tex.width, tex.height, 1, IL_RGB, IL_FLOAT, tex.data.data());
		ilDeleteImages(1, &ImageName); // Delete the image name. 
	}

	texture posx, negx, posy, negy, posz, negz;
};

#endif
