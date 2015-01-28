#ifndef DEPTH_READER_H
#define DEPTH_READER_H

#include <glm/glm.hpp>

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include "FramebufferObject.h"

class DepthReader
{
public:
	int width;
	int height;

	FramebufferObject* fbo;
	GLuint depth_buffer;
	GLuint color_buffer;

	// to read back the results:
	unsigned char* data;
	GLuint pbo;

	DepthReader();

	float readPixels( const glm::vec2& cursorPos );
};

#endif
