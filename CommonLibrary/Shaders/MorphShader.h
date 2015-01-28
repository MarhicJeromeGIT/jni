#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************
class MorphShader : public Shader
{
	friend class ShaderManager;
private:
	MorphShader(void);
	~MorphShader(void);
public:
	UniformVec4f*  uDiffuseColor;
	UniformFloat*  uMorphCoefficient;
	UniformSampler2D*  uTextureSampler;

	UniformMat3f*  uNormalMatrix;
	UniformVec4f*  uLightPosition;
	UniformVec4f*  uLightDiffuseColor;
	UniformVec4f*  uAmbientColor;

	int vertexAttribLoc;
	int vertexMorphAttribLoc;
	int normalAttribLoc;
	int normalMorphAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};
