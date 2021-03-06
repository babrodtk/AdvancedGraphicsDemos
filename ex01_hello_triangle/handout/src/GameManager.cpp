#include "GameManager.h"
#include "GeometryManager.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GlTools.h"
#include "GameException.h"

using std::cerr;
using std::endl;

GameManager::GameManager() {
	my_timer.restart();
}

GameManager::~GameManager() {
}

void GameManager::createOpenGLContext() {
	//Set OpenGL major an minor versions

	/**
	 * FIXME 1: Set which version of OpenGL to use here (3.1), and allocate
	 * the correct number of bits for the different color components
	 * and depth buffer, etc.
	 */
	std::cerr << "Perform the FIXME tasks." << std::endl;


	// Initalize video
	main_window = SDL_CreateWindow("NITH - PG612 Example OpenGL Program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!main_window) {
		THROW_EXCEPTION("SDL_CreateWindow failed");
	}

	//Create OpenGL context
	main_context = SDL_GL_CreateContext(main_window);

	/**
	 * FIXME 2: Initialize GLEW here. Make sure GLEW is initialized
	 * for a 3.1 context
	 */
}

void GameManager::setOpenGLStates() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.5, 1.0);
}

void GameManager::createMatrices() {
	/**
	 * FIXME 3: Create proper matrices for perspective, model,
	 * and view here using GLM.
	 */
}

void GameManager::createSimpleProgram() {
	/**
	 * FIXME 6: Implement phong shading in these shaders.
	 */
	std::string fs_src = "#version 130\n"
		"in vec3 ex_Color;\n"
		"out vec4 res_Color;\n"
		"\n"
		"void main() {\n"
		"    res_Color = vec4(ex_Color, 1.0f);\n"
		"}\n";

	std::string vs_src = "#version 130\n"
		"uniform mat4 projection;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"in  vec3 in_Position;\n"
		"out vec3 ex_Color;\n"
		"void main() {\n"
		"   vec4 pos = vec4(in_Position, 1.0);\n"
		"	gl_Position = projection * view * model * pos;\n"
		"	ex_Color = vec3(0.5f, 0.5f, 1.0f);\n"
		"}\n";

	//Compile shaders, attach to program object, and link
	GLuint vs = GlTools::compileShader(vs_src, GL_VERTEX_SHADER);
	GLuint fs = GlTools::compileShader(fs_src, GL_FRAGMENT_SHADER);
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	GlTools::linkProgram( program);

	//Set uniforms for the program.
	/**
	 * FIXME 4: Set the uniforms (matrices) used in the shader here
	 */
}

void GameManager::createVAO() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	ASSERT_GL;

	//Generate VBO for the vertex data and upload our data.
	//We have n vertices, each with three coordinates (x, y, z), and
	//each coordinate is sizeof(float) bytes.
	glGenBuffers(1, &vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER,
			GeometryManager::getCubeNVertices() * 3 * sizeof(float),
			GeometryManager::getCubeVertices(), GL_STATIC_DRAW);
	GLint vertexAttribLocation = glGetAttribLocation(program, "in_Position");
	assert(vertexAttribLocation >= 0);
	glVertexAttribPointer(vertexAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexAttribLocation);
	ASSERT_GL;

	//Generate VBO for normal data and upload
	/**
	 * FIXME 6: Create VBO for normal data and upload it
	 */

	//Unbind VBOs and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	ASSERT_GL;
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
	createMatrices();
	
	/**
	  * FIXME 2: This will crash unless you have created a proper
	  * OpenGL 3.3 context. Create the OpenGL context, and then
	  * uncomment the below lines.
	  */
	/*
	createSimpleProgram();
	createVAO();
	*/
}

void GameManager::render() {
	//Get elapsed time
	double elapsed = my_timer.elapsedAndRestart();
	float rotate_degrees = elapsed * 20.0;

	//Clear screen, and set the correct program
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);

	//Rotate cube model
	/**
	 * FIXME 5: Update the model matrix here to rotate the cube
	 * Remember that you have to upload it to the program using
	 * glUniformMatrix...
	 */

	//Create normal matrix, the transpose of the inverse
	//3x3 leading submatrix of the modelview matrix
	/**
	 * FIXME 6: Implement normal matrix calculation here,
	 * and set the normal matrix as input to the shader
	 */

	//Render geometry
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, GeometryManager::getCubeNVertices());
	glUseProgram(0);
	ASSERT_GL;
}

void GameManager::play() {
	bool doExit = false;

	//SDL main loop
	while (!doExit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {// poll for pending events
			switch (event.type) {
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
		/**
		  * FIXME 4: The render function will seg fault unless you have a 
		  * proper OpenGL context and created a proper program with the
		  * correct inputs. Uncomment to make sure the render function is caled
		  */
		/*
		render();
		*/
		SDL_GL_SwapWindow(main_window);
	}
	quit();
}

void GameManager::quit() {
	std::cout << "Bye bye..." << std::endl;
}
