#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************
class DiffuseShader : public Shader
{
	friend class ShaderManager;
private:
	DiffuseShader(void);
	~DiffuseShader(void);
public:
	UniformVec4f*  uDiffuseColor;

	int vertexAttribLoc;

	void load();
	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};

//****************************************************************************
// A diffuse color shader with shadows
//
//****************************************************************************
class DiffuseShadowShader : public Shader
{
public:
	DiffuseShadowShader(void);
	~DiffuseShadowShader(void);

	UniformSampler2D* shadowMapUniform;
	UniformMat4f* lightMapUniform;
	UniformVec4f*  uDiffuseColor;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};
