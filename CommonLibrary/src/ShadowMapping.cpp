#include "ShadowMapping.h"

#include "TextureGL.h"
#include <iostream>
using namespace std;

ShadowMapping::ShadowMapping(int width, int height)
{	
	glGenTextures(1, &shadow_texture);
	glBindTexture(GL_TEXTURE_2D, shadow_texture);

#ifdef __ANDROID__
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, width, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, width, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
#endif
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
#ifdef __ANDROID__
	assert(false);
	//output = TextureGL_CreateEmptyTexture(GL_TEXTURE_2D, width, width, GL_FLOAT, GL_RGBA);
#else
	assert(false);
//	output = TextureGL_CreateEmptyTexture(GL_TEXTURE_2D, width, width, GL_RGBA32F, GL_RGBA);
#endif
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	fbo = new FramebufferObject();
	//fbo->AttachTexture(GL_TEXTURE_2D, output, GL_COLOR_ATTACHMENT0);
	fbo->AttachTexture(GL_TEXTURE_2D, 0, GL_COLOR_ATTACHMENT0);
	//glDrawBuffer(GL_NONE);
	fbo->AttachTexture(GL_TEXTURE_2D, shadow_texture, GL_DEPTH_ATTACHMENT );
	fbo->Disable();

	/*
	//glDrawBuffer(GL_NONE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	fbo->Disable();


	// Create the FBO
	fbo = new FramebufferObject();
	
	output = TextureGL_CreateEmptyTexture(GL_TEXTURE_2D, width, width, GL_RGBA16F, GL_RGBA);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	fbo->AttachTexture(GL_TEXTURE_2D, output, GL_COLOR_ATTACHMENT0);

    // Create the depth buffer
    glGenTextures(1, &shadow_texture);
    glBindTexture(GL_TEXTURE_2D, shadow_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	fbo->Bind();
 //   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_texture, 0);

    // Disable writes to the color buffer
    //glDrawBuffer(GL_NONE);
	fbo->Disable();

	glBindTexture(GL_TEXTURE_2D, 0);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return;
    }
*/
	cout<<"fbo valide ? "<<fbo->IsValid()<<endl;
}


ShadowMapping::~ShadowMapping(void)
{
}
