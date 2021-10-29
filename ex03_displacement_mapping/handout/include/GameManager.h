#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#include <tuple>
#include <vector>

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
	 * Creates the matrices for the OpenGL transformations,
	 * perspective, etc.
	 */
	void createMatrices();

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
	GLuint vertex_bo; //< VBO for vertex data
	GLuint index_bo; //< VBO for vertex data
	GLuint height_texture, color_texture; //<! Our heightmap texture
	
	std::shared_ptr<GLUtils::Program> program;

	Timer my_timer; //< Timer for machine independent motion

	glm::mat4 projection_matrix; //< OpenGL projection matrix
	glm::mat4 model_matrix; //< OpenGL model transformation matrix
	glm::mat4 view_matrix; //< OpenGL camera/view matrix
	glm::mat3 normal_matrix; //< OpenGL matrix to transfor normals

	SDL_Window* main_window; //< Our window handle
	SDL_GLContext main_context; //< Our opengl context handle 
	
	struct TerrainMesh {
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		unsigned int restart_token;
	};

	TerrainMesh mesh;

	/**
	 * Creates a mesh for us using triangle strips
	 * @param nx Number of elements in x direction
	 * @param ny Number of elements in y direction
	 * @param restart_token the integer in the index list that corresponds to primitive restart
	 * @return a tuple consisting of the vertex data, index data, and the restart index.
	 */
	TerrainMesh createTriangleStripMesh(unsigned int nx, unsigned int ny);

	/**
	 * Creates a mesh for us using triangle strips
	 * @param nx Number of elements in x direction
	 * @param ny Number of elements in y direction
	 * @param restart_token the integer in the index list that corresponds to primitive restart
	 * @return a tuple consisting of the vertex data, index data, and the restart index.
	 */
	TerrainMesh createTriangleFanMesh(unsigned int nx, unsigned int ny);

	/**
	  * Loads a texture into the OpenGL texture texture
	  */
	static GLuint loadTexture(std::string filename);
};

#endif // _GAMEMANAGER_H_
