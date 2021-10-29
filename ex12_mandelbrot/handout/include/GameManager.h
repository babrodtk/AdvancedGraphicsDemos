#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#include <memory>

#include <GL/glew.h>
#include <SDL.h>
#include <glm/glm.hpp>

#include "Timer.h"
#include "GLUtils/GLUtils.hpp"

/**
 * This class handles the game logic and display.
 * Uses SDL as the display manager, and glm for 
 * vector and matrix computations
 */
class GameManager {
public:

	/**
	 * Constructor
	 */
	GameManager();

	/**
	 * Destructor
	 */
	~GameManager();

	/**
	 * Initializes the game, including the OpenGL context
	 * and data required
	 */
	void init();

	/**
	 * The main loop of the game. Runs the SDL main loop
	 */
	void play();

	/**
	 * Quit function
	 */
	void quit();

	/**
	 * Function that handles rendering into the OpenGL context
	 */
	void render();

protected:
	/**
	 * Creates the OpenGL context using SDL
	 */
	void createOpenGLContext();

	/**
	 * Sets states for OpenGL that we want to keep persistent
	 * throughout the game
	 */
	void setOpenGLStates();

	/**
	 * Compiles, attaches, links, and sets uniforms for
	 * a simple OpenGL program
	 */
	void createSimpleProgram();

	/**
	 * Creates vertex array objects
	 */
	void createVAO();
	
	static const unsigned int window_width = 800;
	static const unsigned int window_height = 600;

private:
	GLuint vao; //< Vertex array object
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > vertices;
	std::shared_ptr<GLUtils::BO<GL_ELEMENT_ARRAY_BUFFER> > indices;
	std::shared_ptr<GLUtils::Program> mandelbrot_program;

	Timer my_timer; //< Timer for machine independent motion

	static GLubyte quad_indices[];
	static GLfloat quad_vertices[];

	glm::vec2 mouse_start;
	glm::vec2 position_start;
	glm::vec2 position;
	float zoom;
	bool moving;
	float iterations;
	
	SDL_Window* main_window; //< Our window handle
	SDL_GLContext main_context; //< Our opengl context handle 
};

#endif // _GAMEMANAGER_H_
