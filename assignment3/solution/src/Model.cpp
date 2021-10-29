#include "Model.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "RayTracerState.hpp"
#include "SceneObjectEffect.hpp"

inline glm::vec3 toVec3(aiVector3D& in) {
	glm::vec3 out;
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
	return out;
}

Model::Model(std::string filename, glm::vec3 origin, float scale, std::shared_ptr<SceneObjectEffect> effect) {
	struct aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_Quality | aiProcess_Triangulate);// | aiProcess_FlipWindingOrder);
	if (!scene) {
		std::string log = "Unable to load mesh from ";
		log.append(filename);
		throw std::runtime_error(log);
	}

	//Load the model recursively into data
	min_dim = glm::vec3(std::numeric_limits<float>::max());
	max_dim = glm::vec3(std::numeric_limits<float>::min());
	findBBoxRecursive(scene, scene->mRootNode, &trafo, min_dim, max_dim);

	//Find translation and scale to center model by transforming the incoming ray
	translation = (max_dim - min_dim) / glm::vec3(2.0f) + min_dim + origin;
	glm::vec3 scale_helper = (max_dim - min_dim);
	this->scale = std::min(scale_helper.x, std::min(scale_helper.y, scale_helper.z))/scale;
	this->effect = effect;
}

Model::~Model() {

}

void Model::findBBoxRecursive(const aiScene* scene, const aiNode* node, aiMatrix4x4* trafo,
	glm::vec3& min_dim, glm::vec3& max_dim) {
	struct aiMatrix4x4 prev;

	prev = *trafo;
	aiMultiplyMatrix4(trafo, &node->mTransformation);

	for (unsigned int n=0; n < node->mNumMeshes; ++n) {
		const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[n]];
		for (unsigned int t = 0; t < mesh->mNumVertices; ++t) {

			struct aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp,trafo);

			min_dim.x = std::min(min_dim.x,tmp.x);
			min_dim.y = std::min(min_dim.y,tmp.y);
			min_dim.z = std::min(min_dim.z,tmp.z);

			max_dim.x = std::max(max_dim.x,tmp.x);
			max_dim.y = std::max(max_dim.y,tmp.y);
			max_dim.z = std::max(max_dim.z,tmp.z);
		}
	}

	for (unsigned int n = 0; n < node->mNumChildren; ++n)
		findBBoxRecursive(scene, node->mChildren[n],trafo,min_dim,max_dim);
	*trafo = prev;
}

glm::vec3 Model::rayTrace(Ray &ray, const float& t, RayTracerState& state) {
#ifdef _OPENMP
	int id = omp_get_thread_num();
#else
	int id = 0;
#endif
	glm::vec3 out_color(0.5f);
	struct aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	Triangle* tri = &cache[id];
	
	glm::vec3 n = glm::vec3(0.0, 0.0, 1.0);
	if (tri->mesh->HasNormals()) {
		n = glm::normalize(
		  toVec3(tri->mesh->mNormals[tri->face->mIndices[0]])
		+ toVec3(tri->mesh->mNormals[tri->face->mIndices[1]])
		+ toVec3(tri->mesh->mNormals[tri->face->mIndices[2]]));
	}

	return effect->rayTrace(ray, t, n, state);
}

float Model::intersect(const Ray& input_r) {
#ifdef _OPENMP
	int id = omp_get_thread_num();
#else
	int id = 0;
#endif
	float t_min = std::numeric_limits<float>::max();
	Triangle tri;
	struct aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	bool intersect = false;

	Ray r_m = worldToModel(input_r);

	intersectRecursive(scene, scene->mRootNode, &trafo, r_m, t_min, tri);
	if (t_min < std::numeric_limits<float>::max()) {
		cache[id] = tri;
		return t_min;
	}
	else {
		return -1;
	}
}



/**
  * Adapted from
  * http://www.blackpawn.com/texts/pointinpoly/default.html and
  * http://en.wikipedia.org/wiki/Line-plane_intersection
  */
float Model::intersect(const aiMesh* mesh, const aiFace* face, const Ray& r) {
	float t;
	const float z_offset = 10e-4f;

	//Indices to vertices for face
	int ia = face->mIndices[0];
	int ib = face->mIndices[1];
	int ic = face->mIndices[2];
			
	//Get the three vertices
	glm::vec3 a = toVec3(mesh->mVertices[ia]);
	glm::vec3 b = toVec3(mesh->mVertices[ib]);
	glm::vec3 c = toVec3(mesh->mVertices[ic]);

	//Compute the two vectors of the triangle from a, and its normal
	glm::vec3 v0 = c-a;
	glm::vec3 v1 = b-a;
	glm::vec3 n = glm::cross(v0, v1);

	//Compute the intersection point of our ray and the plane defined by the normal and a:
	t = glm::dot(a - r.getOrigin(), n) / glm::dot(r.getDirection(), n);
	if (t < z_offset) return -1.0; //intersection in negative direction, just return before continuing

	//Now find the intersection point and define the vector from a
	glm::vec3 p = r.getOrigin() + t*r.getDirection();
	glm::vec3 v2 = p-a;

	//Compute dot products
	float dot00 = glm::dot(v0, v0);
	float dot01 = glm::dot(v0, v1);
	float dot02 = glm::dot(v0, v2);
	float dot11 = glm::dot(v1, v1);
	float dot12 = glm::dot(v1, v2);

	//Compute barycentric coordinates
	float q = 1 / (dot00*dot11 - dot01*dot01);
	float u = (dot11*dot02 - dot01*dot12)*q;
	float v = (dot00*dot12 - dot01*dot02)*q;

	if ((u > 0) && (v > 0) && (u + v < 1)) {
		return t;
	}
	else {
		return -1.0f;
	}
}

void Model::intersectRecursive(const aiScene* scene, const aiNode* node, aiMatrix4x4* trafo, 
		const Ray& r, float& t_min, Triangle& tri) {
	const float z_offset = 10e-4f;
	struct aiMatrix4x4 prev;
	prev = *trafo;
	aiMultiplyMatrix4(trafo, &node->mTransformation);

	// draw all meshes assigned to this node
	for (unsigned int n=0; n < node->mNumMeshes; ++n) {
		const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[n]];
		for (unsigned int k = 0; k < mesh->mNumFaces; ++k) {
			const struct aiFace* face = &mesh->mFaces[k];

			if(face->mNumIndices != 3) {
				std::cout << "Vertex count for face was " << face->mNumIndices << ", expected 3. Skipping face" << std::endl;
				continue;
			}

			float t = intersect(mesh, face, r);
			if (t > z_offset && t < t_min) {
				t_min = t;
				tri.face = face;
				tri.mesh = mesh;
			}
		}
	}

	// Check for intersection for all children
	for (int n = 0; n < node->mNumChildren; ++n) {
		intersectRecursive(scene, node->mChildren[n], trafo, r, t_min, tri);
	}
	*trafo = prev;
}