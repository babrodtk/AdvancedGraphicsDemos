#ifndef _GLTOOLS_H_
#define _GLTOOLS_H_
#include <sstream>
#include <string>
#include <GL/glew.h>

//Simple define that allows us to set buffer offsets for VBO rendering
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

/**
 * Simple assert that checks for OpenGL errors, and throws
 * a runtime error if anything goes wrong
 */
#define ASSERT_GL do { \
    GLenum ASSERT_GL_err = glGetError(); \
    if( ASSERT_GL_err != GL_NO_ERROR ) { \
		std::stringstream ASSERT_GL_string; \
		ASSERT_GL_string << __FILE__ << '@' << __LINE__ << ": OpenGL error:" \
             << std::hex << ASSERT_GL_err << " " << gluErrorString(ASSERT_GL_err); \
			 throw std::runtime_error( ASSERT_GL_string.str() ); \
    } \
} while(0);

namespace GlTools {
/**
 * Compiles a shader string into a shader object
 * @param src Shader source code
 * @param type Fragment, geometry or vertex shader?
 * @return name of the compiled shader
 */
GLuint compileShader(std::string& src, GLenum type);

/**
 * Links a shader program
 * @param program The name of the program to link
 */
void linkProgram(GLuint program);

/**
 * Reads a shader source from file
 * @param file The filename (including path) of the file to read
 * @return The file contents as a string.
 */
std::string readFile(std::string file);

} // namespace GlTools

#endif // _GLTOOLS_H_
