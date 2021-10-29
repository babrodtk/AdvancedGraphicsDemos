#include "GlTools.h"

#include <cstdlib>
#include <sstream>
#include <vector>
#include <assert.h>
#include <iostream>
#include <fstream>

using std::cerr;
using std::endl;

namespace GlTools {

GLuint compileShader(std::string& src, GLenum type) {
	// create shader object
	GLuint s = glCreateShader(type);
	if (s == 0) {
		cerr << "Failed to create shader of type " << type << endl;
		exit( EXIT_FAILURE);
	}

	// set source code and compile
	const GLchar* src_list[1] = { src.c_str() };
	glShaderSource(s, 1, src_list, NULL);
	glCompileShader(s);

	// check for errors
	GLint compile_status;
	glGetShaderiv(s, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		// compilation failed
		cerr << "Compilation failed!" << endl;
		cerr << "--- source code ---" << endl;
		cerr << src << endl;

		GLint logsize;
		glGetShaderiv(s, GL_INFO_LOG_LENGTH, &logsize);

		if (logsize > 0) {
			std::vector < GLchar > infolog(logsize + 1);
			glGetShaderInfoLog(s, logsize, NULL, &infolog[0]);

			cerr << "--- error log ---" << endl;
			cerr << std::string(infolog.begin(), infolog.end()) << endl;
		} else {
			cerr << "--- empty log message ---" << endl;
		}
		cerr << "Exiting." << endl;
		exit( EXIT_FAILURE);
	}
	return s;
}


void linkProgram(GLuint program) {
	// link
	glLinkProgram(program);

	// check for errors
	GLint linkstatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkstatus);
	if (linkstatus != GL_TRUE) {
		cerr << "Linking failed!" << endl;

		GLint logsize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logsize);

		if (logsize > 0) {
			std::vector < GLchar > infolog(logsize + 1);
			glGetProgramInfoLog(program, logsize, NULL, &infolog[0]);
			cerr << "--- error log ---" << endl;
			cerr << std::string(infolog.begin(), infolog.end()) << endl;
		} else {
			cerr << "--- empty log message ---" << endl;
		}
		cerr << "Exiting." << endl;
		exit( EXIT_FAILURE);
	}
}


std::string readFile(std::string file) {
	int length;
	std::string buffer;
	std::string contents;

	std::ifstream is;
	is.open(file.c_str(), std::ios::binary );

	// get length of file:
	is.seekg (0, std::ios::end);
	length = is.tellg();
	is.seekg (0, std::ios::beg);

	// reserve memory:
	contents.reserve(length);

	// read data
	while(getline(is,buffer)) {
		contents.append(buffer);
		contents.append("\n");
	}
	is.close();

	return buffer;
}

} // namespace GlTools
