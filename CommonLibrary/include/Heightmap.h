#pragma once

#include <iostream>
#include <stdlib.h>

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include <time.h>
#include <glm/glm.hpp>
using namespace std;
#include "Plane.h"
#include "Model.h"
#include "TextureGL.h"
#include "tga.h"

#include "OpenGLModel.h"

class Heightmap
{
public:
	int width;
	int height;

	glm::vec3 position;
	glm::vec3 scale;

	glm::vec3** heightdata;
	glm::vec3* normales;

	float maxHeight;

	Model* model;
	Heightmap( const char* filename, const glm::vec3& position, const glm::vec3& scale );
	void draw();
	float getHeight( float x, float z);
};

class Terrain : public OpenGLStaticModelInstance
{
public:
	Heightmap* heightmap;

public:
	Terrain(Heightmap* heightmap, const glm::mat4& mat, OpenGLModel* glModel, const Model* internModel );
	void Draw(MATERIAL_DRAW_PASS Pass);
	bool getIntersection( const glm::vec3& rayStart, const glm::vec3& rayDir, glm::vec3& intersectionPoint );
};
