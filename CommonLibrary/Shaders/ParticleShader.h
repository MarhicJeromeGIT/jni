#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// A Particle Shader
//
//****************************************************************************
class ParticleShader : public Shader
{
	friend class ShaderManager;
private:
	ParticleShader(void);
	~ParticleShader(void);

public:

	int vertexAttribLoc;
	int speedAttribLoc;
	int texCoordAttribLoc;

	UniformSampler2D* textureUniform;
	UniformFloat*     uParticleSize;

	void load();
	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};

