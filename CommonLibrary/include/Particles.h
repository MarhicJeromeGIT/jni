#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#define GLM_SWIZZLE

#include "Mesh.h"
#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include "Material.h"
class DynamicMesh;
#include "Renderer.h"

class ParticleSystem
{
public:
	DynamicMesh* particles;
	Material*    particleMat;
public:
	int          nbParticles;
	int			 lastParticleIndex;
	float        nbParticlesToGenerate;

	glm::vec4*  positions; // age component is the lifespand in [0;1]
	glm::vec4*  speed; // w component is the rotation angle in radians
	glm::vec2*  uvCoords;
	unsigned short*    triangles;

	bool isTransparent;
	TRANSPARENCY_MODE transparencyMode;

	bool needZSort;
	float currentTime;
	float lifeTime;

	bool active;
	glm::vec3 position;

	float weight;
	float verticalSpeed;

public:
	ParticleSystem(int numPart);
	virtual void reloadGLData();

	virtual void initParticles();
	virtual void update(float dt);
	virtual void Draw( MATERIAL_DRAW_PASS Pass );
	virtual void activate(const glm::vec3& pos);
};

#endif
