#include "GameManager.h"
#include "GameException.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdexcept>

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

GameManager::GameManager() {
	position_start = glm::vec2(0.0f);
	mouse_start = glm::vec2(0.0f);
	zoom = 1.0f;
	moving = false;
	iterations = 128;
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

	//Set that SDL can update the frame at any time
	SDL_GL_SetSwapInterval(0);
}

void GameManager::setOpenGLStates() {
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	CHECK_GL_ERRORS();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void GameManager::createSimpleProgram() {
	//Compile shaders, attach to program object, and link
	mandelbrot_program.reset(new Program("shaders/mandelbrot.vert", "shaders/mandelbrot.frag"));
	CHECK_GL_ERRORS();

	//Set uniforms for the programs
	mandelbrot_program->use();
	glUniform2fv(mandelbrot_program->getUniform("position"), 1, glm::value_ptr(position));
	glUniform1f(mandelbrot_program->getUniform("zoom"), zoom);
	glUniform1f(mandelbrot_program->getUniform("iterations"), iterations);
	mandelbrot_program->disuse();
	CHECK_GL_ERRORS();
}

void GameManager::createVAO() {
	glGenVertexArrays(1, &vao);

	//Load the quad into vao 1
	glBindVertexArray(vao);
	vertices.reset(new BO<GL_ARRAY_BUFFER>(quad_vertices, sizeof(quad_vertices)));
	indices.reset(new BO<GL_ELEMENT_ARRAY_BUFFER>(quad_indices, sizeof(quad_indices)));
	vertices->bind();
	mandelbrot_program->setAttributePointer("v_pos", 2);
	indices->bind();
	CHECK_GL_ERRORS();

	//Unbind and check for errors
	vertices->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
	CHECK_GL_ERRORS();
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
	createSimpleProgram();
	createVAO();
}

void GameManager::render() {
	//Render quad to screen
	mandelbrot_program->use();
	glUniform2fv(mandelbrot_program->getUniform("position"), 1, glm::value_ptr(position));
	glUniform1f(mandelbrot_program->getUniform("zoom"), zoom);
	glUniform1f(mandelbrot_program->getUniform("iterations"), iterations);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	CHECK_GL_ERRORS();
	glBindVertexArray(0);
	CHECK_GL_ERRORS();

	std::cout << 1.0 / my_timer.elapsedAndRestart() << "\r" << std::flush;
}

void GameManager::play() {
	bool doExit = false;

	//SDL main loop
	while (!doExit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {// poll for pending events
			switch (event.type) {
			case SDL_MOUSEWHEEL:
				if (event.wheel.y > 0) {
					zoom *= 1.0/1.1;
				}
				else {
					zoom *= 1.1;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				moving = true;
				mouse_start = glm::vec2(-event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEBUTTONUP:
				moving = false;
				position_start = position;
				break;
			case SDL_MOUSEMOTION:
				if (moving) {
					position = position_start + 2.0f*(glm::vec2(-event.motion.x, event.motion.y) - mouse_start)/static_cast<float>(window_width)*zoom;
					std::cout << position.x << ", " << position.y << std::endl;
				}
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym) {
				case SDLK_ESCAPE: //Esc
					doExit = true;
					break;
				case SDLK_PAGEDOWN:
					zoom *= 0.9; break;
				case SDLK_PAGEUP:
					zoom *= 1.1; break;
				case SDLK_q:
					if (event.key.keysym.mod & KMOD_CTRL) {//Ctrl+q
						doExit = true;
					}
					break;
				case SDLK_PLUS:
					iterations = iterations*1.1;
					std::cout << iterations << " iterations" << std::endl;
					break;
				case SDLK_MINUS:
					iterations = iterations*1.0/1.1;
					std::cout << iterations << " iterations" << std::endl;
				}
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
