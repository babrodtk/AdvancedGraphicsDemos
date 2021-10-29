#include "GameManager.h"
#include "GameException.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdexcept>

#include "GLUtils/GLUtils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using std::cerr;
using std::endl;
using GLUtils::BO;
using GLUtils::Program;
using GLUtils::readFile;

//Vertices to render a quad
GLfloat GameManager::quad_vertices[] =  {
	-1.f, -1.f,
	 1.f, -1.f,
	 1.f,  1.f,
	-1.f,  1.f,
};

//Indices to create the quad
GLubyte GameManager::quad_indices[] = {
	0, 1, 2, //triangle 1
	2, 3, 0, //triangle 2
};

unsigned int GameManager::downscale_level = 4;

GameManager::GameManager() {
	my_timer.restart();
}

GameManager::~GameManager() {
}

void GameManager::createOpenGLContext() {
	//Set OpenGL major an minor versions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Set OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Use double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); // Use framebuffer with 16 bit depth buffer
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Use framebuffer with 8 bit for red
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); // Use framebuffer with 8 bit for green
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8); // Use framebuffer with 8 bit for blue
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // Use framebuffer with 8 bit for alpha
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	// Initalize video
	main_window = SDL_CreateWindow("NITH - PG612 Example OpenGL Program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!main_window) {
		THROW_EXCEPTION("SDL_CreateWindow failed");
	}

	//Create OpenGL context
	main_context = SDL_GL_CreateContext(main_window);
	trackball.setWindowSize(window_width, window_height);

	// Init glew
	// glewExperimental is required in openGL 3.1 
	// to create forward compatible contexts 
	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		std::stringstream err;
		err << "Error initializing GLEW: " << glewGetErrorString(glewErr);
		THROW_EXCEPTION(err.str());
	}

	// Unfortunately glewInit generates an OpenGL error, but does what it's
	// supposed to (setting function pointers for core functionality).
	// Lets do the ugly thing of swallowing the error....
	glGetError();
}

void GameManager::setOpenGLStates() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	CHECK_GL_ERRORS();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GameManager::createMatrices() {
	projection_matrix = glm::perspective(45.0f,
			window_width / (float) window_height, 1.0f, 10.f);
	model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(3));
	view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
}

void GameManager::createSimpleProgram() {
	//Compile shaders, attach to program object, and link
	phong_program.reset(new Program("shaders/phong_os.vert", "shaders/phong_os.frag"));
	bloom_program.reset(new Program("shaders/bloom.vert", "shaders/bloom.frag"));
	horizontal_blur_downscale_program.reset(new Program("shaders/horizontal_downscale.vert", "shaders/horizontal_downscale.frag"));
	vertical_blur_downscale_program.reset(new Program("shaders/vertical_downscale.vert", "shaders/vertical_downscale.frag"));
	CHECK_GL_ERRORS();

	//Set uniforms for the programs
	phong_program->use();
	glUniformMatrix4fv(phong_program->getUniform("projection_matrix"), 1, 0, glm::value_ptr(projection_matrix));
	phong_program->disuse();
	CHECK_GL_ERRORS();
	
	bloom_program->use();
	glUniform1i(bloom_program->getUniform("my_texture"), 0);
	glUniform1i(bloom_program->getUniform("my_bloom_texture"), 1);
	CHECK_GL_ERRORS();
	
	horizontal_blur_downscale_program->use();
	glUniform1i(horizontal_blur_downscale_program->getUniform("my_texture"), 0);
	glUniform1f(horizontal_blur_downscale_program->getUniform("dx"), 1.0/horizontal_blur_downscale->getWidth());
	CHECK_GL_ERRORS();
	
	vertical_blur_downscale_program->use();
	glUniform1i(vertical_blur_downscale_program->getUniform("my_texture"), 1);
	glUniform1f(vertical_blur_downscale_program->getUniform("dy"), 1.0/vertical_blur_downscale->getHeight());
	CHECK_GL_ERRORS();
}

void GameManager::createVAO() {
	glGenVertexArrays(max_vaos, vaos);

	//Load a model into vao 0
	glBindVertexArray(vaos[0]);
	CHECK_GL_ERRORS();
	model.reset(new Model("models/bunny.obj", false));
	model->getVertices()->bind();
	phong_program->setAttributePointer("position", 3);
	CHECK_GL_ERRORS();
	model->getNormals()->bind();
	phong_program->setAttributePointer("normal", 3);
	CHECK_GL_ERRORS();
	vertices->unbind(); //Unbinds both vertices and normals

	//Load the quad into vao 1
	glBindVertexArray(vaos[1]);
	vertices.reset(new BO<GL_ARRAY_BUFFER>(quad_vertices, sizeof(quad_vertices)));
	indices.reset(new BO<GL_ELEMENT_ARRAY_BUFFER>(quad_indices, sizeof(quad_indices)));
	vertices->bind();
	bloom_program->setAttributePointer("position", 2);
	indices->bind();
	CHECK_GL_ERRORS();

	//Unbind and check for errors
	vertices->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
	CHECK_GL_ERRORS();
}

void GameManager::createFBO() {
	//fbo.reset(new TextureFBO(256, 256));
	fbo.reset(new TextureFBO(window_width, window_height));
	fbo->unbind();

	horizontal_blur_downscale.reset(new TextureFBO(window_width >> downscale_level, window_height >> downscale_level));
	horizontal_blur_downscale->unbind();

	vertical_blur_downscale.reset(new TextureFBO(window_width >> downscale_level, window_height >> downscale_level));
	vertical_blur_downscale->unbind();
}

void GameManager::init() {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::stringstream err;
		err << "Could not initialize SDL: " << SDL_GetError();
		THROW_EXCEPTION(err.str());
	}
	atexit(SDL_Quit);

	createOpenGLContext();
	setOpenGLStates();
	createFBO();
	createMatrices();
	createSimpleProgram();
	createVAO();
}

void GameManager::renderMeshRecursive(MeshPart& mesh, const std::shared_ptr<Program>& program, 
		const glm::mat4& view_matrix, const glm::mat4& model_matrix) {
	//Create modelview matrix
	glm::mat4 meshpart_model_matrix = model_matrix*mesh.transform;
	glm::mat4 modelview_matrix = view_matrix*meshpart_model_matrix;
	glUniformMatrix4fv(program->getUniform("modelview_matrix"), 1, 0, glm::value_ptr(modelview_matrix));
	
	glm::mat4 modelview_inverse_matrix = glm::inverse(glm::mat4(modelview_matrix));
	glUniformMatrix4fv(program->getUniform("modelview_inverse_matrix"), 1, 0, glm::value_ptr(modelview_inverse_matrix));

	//Create normal matrix, the transpose of the inverse
	//3x3 leading submatrix of the modelview matrix
	//glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
	//glUniformMatrix3fv(program->getUniform("normal_matrix"), 1, 0, glm::value_ptr(normal_matrix));
	
	if (mesh.count > 0)
		glDrawArrays(GL_TRIANGLES, mesh.first, mesh.count);
	for (int i=0; i<mesh.children.size(); ++i)
		renderMeshRecursive(mesh.children.at(i), program, view_matrix, meshpart_model_matrix);
}

void GameManager::render() {
	//Clear screen, and set the correct program
	glm::mat4 view_matrix_new = view_matrix*trackball_view_matrix;

	//Render model
	fbo->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, fbo->getWidth(), fbo->getHeight());
	phong_program->use();
	glBindVertexArray(vaos[0]);
	renderMeshRecursive(model->getMesh(), phong_program, view_matrix_new, model_matrix);
	fbo->unbind();
	CHECK_GL_ERRORS();

	//Generate mipmaps
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo->getTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	//Downsample and blur horizontally
	horizontal_blur_downscale->bind();
	glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo->getTexture());
	glViewport(0, 0, horizontal_blur_downscale->getWidth(), horizontal_blur_downscale->getHeight());
	horizontal_blur_downscale_program->use();
	glBindVertexArray(vaos[1]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	glDepthMask(GL_TRUE);
	horizontal_blur_downscale->unbind();
	
	//Downsample and blur vertically
	vertical_blur_downscale->bind();
	glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, horizontal_blur_downscale->getTexture());
	glViewport(0, 0, vertical_blur_downscale->getWidth(), vertical_blur_downscale->getHeight());
	vertical_blur_downscale_program->use();
	glBindVertexArray(vaos[1]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	glDepthMask(GL_TRUE);
	vertical_blur_downscale->unbind();

	//Render quad to screen
	glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, vertical_blur_downscale->getTexture());
	glViewport(0, 0, window_width, window_height);
	bloom_program->use();
	glBindVertexArray(vaos[1]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	glBindTexture(GL_TEXTURE_2D, 0);
	glDepthMask(GL_TRUE);
	CHECK_GL_ERRORS();

	glBindVertexArray(0);
	CHECK_GL_ERRORS();
}

void GameManager::play() {
	bool doExit = false;

	//SDL main loop
	while (!doExit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {// poll for pending events
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				trackball.rotateBegin(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEBUTTONUP:
				trackball.rotateEnd(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEMOTION:
				trackball_view_matrix = trackball.rotate(event.motion.x, event.motion.y);
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) //Esc
					doExit = true;
				if (event.key.keysym.sym == SDLK_q
						&& event.key.keysym.mod & KMOD_CTRL) //Ctrl+q
					doExit = true;
				break;
			case SDL_QUIT: //e.g., user clicks the upper right x
				doExit = true;
				break;
			}
		}

		//Render, and swap front and back buffers
		render();
		SDL_GL_SwapWindow(main_window);
	}
	quit();
}

void GameManager::quit() {
	std::cout << "Bye bye..." << std::endl;
}
