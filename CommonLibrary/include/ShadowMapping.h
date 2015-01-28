#pragma once

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include "FramebufferObject.h"

class ShadowMapping
{
public:
	FramebufferObject* fbo;
	GLuint shadow_texture;
	GLuint output;


public:
	ShadowMapping(int width, int height);
	~ShadowMapping(void);
};


