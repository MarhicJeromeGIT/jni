#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Skybox shader
//
//****************************************************************************

class SkyboxShader : public Shader
{
friend class ShaderManager;

public:
	SkyboxShader(void);
	void load();

	~SkyboxShader(void);

	void enable( const ShaderParams& params );
	void disable();
	virtual void Draw( Mesh* mesh );

	GLint OldCullFaceMode;
	GLint OldDepthFuncMode;

	int vertexAttribLoc;
	UniformSamplerCube* uCubeMapSampler;
};
