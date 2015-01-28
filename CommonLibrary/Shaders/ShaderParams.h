#pragma once

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <string>

//****************************************************************************
// Global parameters for all the shaders (lighting, camera...)
//
//****************************************************************************


#define MAX_LIGHTS 8

struct Light
{
	glm::vec4 lightPosition;
	glm::vec4 lightDiffuseColor;
	glm::vec3 lightSpecularColor;
	float     lightShininess; // specular component
};

// Singleton
class ShaderParams
{
private:
	static ShaderParams* thisObject;
	ShaderParams();

public:
	// View matrices
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
    glm:: mat3 normalMatrix;
	glm::mat4 objectMatrix;
	glm::mat4 modelViewMatrix; // viewMatrix * objectMatrix;

	// Lighting
	Light lights[MAX_LIGHTS];
	glm::mat4 lightMap;
	GLuint shadowmap;

	// reflection:
	GLuint watermap;
	glm::vec4 clipPlane;

	float time; // in seconds

	// ambient color:
 	glm::vec4 ambient;

	// Sound
	int soundVolume;
	static ShaderParams* get();

	// window resolution
	float win_x;
	float win_y;

	std::string currentDir;
};
