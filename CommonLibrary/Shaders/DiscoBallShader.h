#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************
class DiscoBallShader : public Shader
{
	friend class ShaderManager;
private:
	DiscoBallShader(void);
	~DiscoBallShader(void);
public:
	UniformVec4f*  uDiffuseColor;
	UniformFloat*  uTime;
	UniformSampler2D*  uTextureSampler;
	UniformMat3f*  uNormalMatrix;

	int vertexAttribLoc;
	int texCoordAttribLoc;
	int normalAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};
