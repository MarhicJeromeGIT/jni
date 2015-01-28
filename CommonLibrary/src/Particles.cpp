#include "Particles.h"

#include "macros.h" // randf
#include "TextureGL.h"
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#undef GLM_SWIZZLE

using namespace glm;
#include "Material.h"
#include "OpenGLModel.h"
#include "TextureGL.h"

//#define TWEAK_STAR

using namespace glm;

ParticleSystem::ParticleSystem( int numPart)
{
	nbParticles = numPart;
	// 4 vertices, 2 triangles by particle
	particles = NULL;
	position = vec3(0,0,0);

	positions = new vec4[ nbParticles * 4 ];
	uvCoords  = new vec2[ nbParticles * 4 ];
	speed     = new vec4[ nbParticles * 4 ];
	triangles = new unsigned short[ nbParticles * 2 * 3];

	initParticles();

	currentTime = 0.0f;
	lifeTime    = 50.0;

	lastParticleIndex = 0;
	nbParticlesToGenerate = 0;

	needZSort = false;
	active = false;
	position = vec3(0,0,0);

	weight = 12.0f;
	verticalSpeed = 50.0f;

	isTransparent = false;
	transparencyMode = TRANSPARENCY_MODE::GL_ONE_GL_ONE;

#if defined( USE_TWEAKER ) && defined( TWEAK_STAR )
		Tweaker::get()->addTweakable( new FloatTweakable( "vertical speed",
				[this](float f){ this->verticalSpeed = f; }, verticalSpeed, 0.0f,100.0f ));

		Tweaker::get()->addTweakable( new FloatTweakable( "weight",
				[this](float f){ this->weight = f; }, weight, 0.0f,100.0f ));
#endif

}

void ParticleSystem::reloadGLData()
{
	initParticles();
}

void ParticleSystem::initParticles()
{
	if( particles != NULL )
	{
		delete particles;
	}
	particles = new DynamicMesh( nbParticles*4, nbParticles*2 );


	for( int i=0; i< nbParticles; i++ )
	{
		vec4 pos = vec4( position.x, position.y, position.z, 1.0f ); // originally extinct
		positions[i*4]   = pos;
		positions[i*4+1] = pos;
		positions[i*4+2] = pos;
		positions[i*4+3] = pos;

		uvCoords[i*4]   = vec2(0,0);
		uvCoords[i*4+1] = vec2(1,0);
		uvCoords[i*4+2] = vec2(1,1);
		uvCoords[i*4+3] = vec2(0,1);

		vec4 s = vec4( 0.0,0.0,0.0, 0.0 );
		speed[i*4]   = s;
		speed[i*4+1] = s;
		speed[i*4+2] = s;
		speed[i*4+3] = s;

		triangles[i*2*3]   = i*4;
		triangles[i*2*3+1] = i*4+1;
		triangles[i*2*3+2] = i*4+2;

		triangles[(i*2+1)*3]   = i*4;
		triangles[(i*2+1)*3+1] = i*4+2;
		triangles[(i*2+1)*3+2] = i*4+3;
	}

	DynamicMesh::updateBuffer( positions, nbParticles*4,   particles->mesh.vertexBuffer,   false );
	DynamicMesh::updateBuffer( uvCoords,  nbParticles*4,   particles->mesh.texCoordBuffer, false );
	DynamicMesh::updateBuffer( triangles, nbParticles*2*3, particles->mesh.indicesBuffer,  false );
	DynamicMesh::updateBuffer( speed,     nbParticles*4,   particles->mesh.speedBuffer,    true );
}

void ParticleSystem::update(float dt)
{
	if( !active )
		return;

	for( int i=0; i< nbParticles; i++ )
	{
		vec4 s = speed[i*4];
		s.y -= 9.81*dt*weight;
		s.w += dt*2.0f; // rotating
		speed[i*4] = s;
		speed[i*4+1] = s;
		speed[i*4+2] = s;
		speed[i*4+3] = s;

		vec4 pos = positions[i*4];
		pos = vec4( vec3(pos) + vec3(s)*dt, pos.w + dt);
		positions[i*4]   = pos;
		positions[i*4+1] = pos;
		positions[i*4+2] = pos;
		positions[i*4+3] = pos;
	}

	// generate new particles when needed:
	const float nbParticlesPerSecond = ((float)nbParticles)/lifeTime;
	nbParticlesToGenerate += dt * nbParticlesPerSecond;
	while( nbParticlesToGenerate > 1 )
	{
		nbParticlesToGenerate -= 1.0f;
		vec4 pos = vec4( position.x, position.y, position.z, 0.0f );

		positions[lastParticleIndex*4] = pos;
		positions[lastParticleIndex*4+1] = pos;
		positions[lastParticleIndex*4+2] = pos;
		positions[lastParticleIndex*4+3] = pos;

		vec4 s = vec4( -3.0f-randf()*1.0f,0.0,0.0, 0.0 );
		speed[lastParticleIndex*4] = s;
		speed[lastParticleIndex*4+1] = s;
		speed[lastParticleIndex*4+2] = s;
		speed[lastParticleIndex*4+3] = s;

		lastParticleIndex = (lastParticleIndex+1)%nbParticles;
	}

	// update the position and speed buffers data to the GPU
	DynamicMesh::updateBuffer( positions, nbParticles*4,  particles->mesh.vertexBuffer,   false );
	DynamicMesh::updateBuffer( speed,     nbParticles*4,  particles->mesh.speedBuffer,    false );
}

void ParticleSystem::Draw( MATERIAL_DRAW_PASS Pass )
{
	if( Pass != MATERIAL_DRAW_PASS::COLOR )
		return;
	if( !active )
		return;


	Shader* shader = particleMat->getShader(Pass);
	if( shader != NULL )
	{
		DrawCall drawcall;

		drawcall.Pass     = Pass;
		drawcall.modelMat = mat4(1.0);
		drawcall.viewMat  = ShaderParams::get()->viewMatrix;
		drawcall.projMat  = ShaderParams::get()->projectionMatrix;

		drawcall.material = particleMat;
		drawcall.mesh     = &(particles->mesh);
		drawcall.hasTransparency = isTransparent;
		drawcall.transparencyMode = transparencyMode;
		Renderer::get()->draw( drawcall );
	}
}

void ParticleSystem::activate(const vec3& pos)
{
	position = pos;
	active = true;
	currentTime = 0.0f;
	initParticles();
}

