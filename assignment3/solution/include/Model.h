#ifndef _MODEL_H__
#define _MODEL_H__

#include <memory>
#include <string>
#include <vector>
#include <map>

#include <assimp.h>
#include <aiPostProcess.h>
#include <aiScene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneObject.hpp"

class Model : public SceneObject {
public:
	Model(std::string filename, glm::vec3 origin, float scale, std::shared_ptr<SceneObjectEffect> effect);
	~Model();
	float intersect(const Ray& r);
	
	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state);

private:
	struct Triangle {
		Triangle() {
			mesh = NULL;
			face = NULL;
		}
		const aiMesh* mesh;
		const aiFace* face;
	};

	static void findBBoxRecursive(const aiScene* scene, const aiNode* node, aiMatrix4x4* trafo, glm::vec3& min_dim, glm::vec3& max_dim);

	static float intersect(const aiMesh* mesh, const aiFace* face, const Ray& r);
	static void intersectRecursive(const aiScene* scene, const aiNode* node, aiMatrix4x4* trafo, const Ray& r, float& t, Triangle& tri);

	const aiScene* scene;
	
	Ray worldToModel(const Ray& r) const;

	glm::vec3 min_dim;
	glm::vec3 max_dim;
	glm::vec3 translation;
	float scale;

	std::map<int, Triangle> cache;
};

//Scale the ray we intersect with instead of scaling the model...
inline Ray Model::worldToModel(const Ray& r) const {
	glm::vec3 p0 = r.getOrigin();
	glm::vec3 l = r.getDirection();
	p0 = (p0-translation)*scale;
	l = l*scale;
	
	return Ray(p0, l);
}

#endif