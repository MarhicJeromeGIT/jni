#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Laser Shader
//
//****************************************************************************
class LaserShader : public Shader
{
	friend class ShaderManager;
private:
	LaserShader(void);
	~LaserShader(void);

public:

	UniformMat3f*      uNormalMat;
	UniformFloat*	   uTimeUniform;

	int vertexAttribLoc;
	int texCoordAttribLoc;
	int normalAttribLoc;

	virtual void load();
	virtual void enable( const ShaderParams& params );
	virtual void disable();
	virtual void Draw( Mesh* mesh);
};

//****************************************************************************
// Laser Shader with dummy vertex shader
//
//****************************************************************************

class LaserShaderDummyVertex : public Shader
{
	friend class ShaderManager;
private:
	LaserShaderDummyVertex(void);
	~LaserShaderDummyVertex(void);

public:

	UniformFloat*	   uTimeUniform;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void load();
	virtual void enable( const ShaderParams& params );
	virtual void disable();
	virtual void Draw( Mesh* mesh);
};
