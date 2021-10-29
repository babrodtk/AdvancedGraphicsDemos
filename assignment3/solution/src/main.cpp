#include <iostream>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "RayTracer.h"
#include "Sphere.hpp"
#include "CubeMap.hpp"
#include "Model.h"
#include "Timer.h"

/**
 * Simple program that starts our game manager
 */
int main(int argc, char *argv[]) {
	try {
		RayTracer* rt;
		Timer t;
		rt = new RayTracer(8000, 8000);
		
		std::shared_ptr<SceneObjectEffect> fresnel(new FresnelEffect());
		std::shared_ptr<SceneObjectEffect> steel(new SteelEffect());
		std::shared_ptr<SceneObjectEffect> phong(new PhongEffect(glm::vec3(0.0, 0.0, 0.0)));
		
		std::shared_ptr<SceneObject> s0(new CubeMap(
			"cubemaps/SaintLazarusChurch3/posx.jpg", "cubemaps/SaintLazarusChurch3/negx.jpg",
			"cubemaps/SaintLazarusChurch3/posy.jpg", "cubemaps/SaintLazarusChurch3/negy.jpg",
			"cubemaps/SaintLazarusChurch3/posz.jpg", "cubemaps/SaintLazarusChurch3/negz.jpg"));
		rt->addSceneObject(s0);
		std::shared_ptr<SceneObject> s1(new Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 3.0f, fresnel));
		rt->addSceneObject(s1);
		/*
		std::shared_ptr<SceneObject> s2(new Sphere(glm::vec3(-3.0f, -2.0f, 3.0f), 2.0f, steel));
		rt->addSceneObject(s2);
		std::shared_ptr<SceneObject> s3(new Sphere(glm::vec3(0.0f, 2.0f, 0.0f), 2.0f, phong));
		rt->addSceneObject(s3);
		std::shared_ptr<SceneObject> s4(new Sphere(glm::vec3(0.0f, 0.0f, 6.0f), 3.0f, steel));
		rt->addSceneObject(s4);
		std::shared_ptr<SceneObject> s5(new Model("models/icosahedron.obj", glm::vec3(0.0f, 0.0f, -3.0f), 4.0, fresnel));
		rt->addSceneObject(s5);*/
		std::shared_ptr<SceneObject> s6(new Sphere(glm::vec3(-3.5f, 3.5f, -3.0f), 2.0f, steel));
		rt->addSceneObject(s6);
		std::shared_ptr<SceneObject> s7(new Sphere(glm::vec3(3.5f, -3.5f, 3.0f), 2.5f, steel));
		rt->addSceneObject(s7);
		std::shared_ptr<SceneObject> s8(new Sphere(glm::vec3(-4.0f, -2.0f, 6.0f), 2.5f, steel));
		rt->addSceneObject(s8);
		std::shared_ptr<SceneObject> s9(new Sphere(glm::vec3(4.0f, 2.0f, 9.0f), 2.5f, steel));
		rt->addSceneObject(s9);
		/*
		std::shared_ptr<SceneObject> s10(new Sphere(glm::vec3(0.0f, 3.0f, 9.0f), 2.0f, phong));
		rt->addSceneObject(s10);
		*/
				
		t.restart();
		rt->render();
		double elapsed = t.elapsed();
		std::cout << "Computed in " << elapsed << " seconds" <<  std::endl;
		rt->save("test", "jpg");

		delete rt;
	} catch (std::exception &e) {
		std::string err = e.what();
		std::cout << err.c_str() << std::endl;
		return -1;
	}
	return 0;
}
