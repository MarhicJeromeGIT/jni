#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// FlameThrowerShader Shader
//
//****************************************************************************
class FlameThrowerShader : public Shader
{
	friend class ShaderManager;
private:
	FlameThrowerShader(void);
	~FlameThrowerShader(void);

public:

	UniformFloat*	   uTimeUniform;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void load();
	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};

//****************************************************************************
// FlameThrowerShader Shader with dummy vertex shader
//
//****************************************************************************
class FlameThrowerShaderSimple : public Shader
{
	friend class ShaderManager;
private:
	FlameThrowerShaderSimple(void);
	~FlameThrowerShaderSimple(void);

public:

	UniformFloat*	   uTimeUniform;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void load();
	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};
