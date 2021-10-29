#include "ShadowFBO.h"
#include "GLUtils/GLUtils.hpp"


ShadowFBO::ShadowFBO(unsigned int width, unsigned int height) {
	this->width = width;
	this->height = height;

	// Initialize Depth Texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Create FBO and attach buffers
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_NONE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CHECK_GL_ERRORS();
	CHECK_GL_FBO_COMPLETENESS();
}

ShadowFBO::~ShadowFBO() {
	glDeleteFramebuffersEXT(1, &fbo);
}

void ShadowFBO::bind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
}

void ShadowFBO::unbind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}