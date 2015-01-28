#include "RayPicking.h"

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include "Object3D.h"

using namespace glm;

RayPicking::RayPicking(void)
{
	rayStart = vec3(0);
	rayEnd   = vec3(0);
}


RayPicking::~RayPicking(void)
{
}

void RayPicking::drawRay()
{
#ifndef __ANDROID__
	glColor3f(1,0,0);
	glBegin(GL_LINES);
	glVertex3f(rayStart.x, rayStart.y, rayStart.z);
	glVertex3f(rayEnd.x, rayEnd.y, rayEnd.z );
	glEnd();
#endif
}

