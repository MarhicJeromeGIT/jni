#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Cel shader with texture
//
//****************************************************************************
class CelShader : public Shader
{
	friend class ShaderManager;
private:
	CelShader(void);
	~CelShader(void);

public:

	UniformMat3f*  uNormalMatrix;
	UniformVec4f*  uDiffuseColor;
	UniformVec4f*  uLightPosition;
	UniformVec4f*  uLightDiffuseColor;
	UniformVec4f*  uAmbientColor;
	UniformSampler2D*  uTextureSampler;
	UniformBoolean* uUseScreenCoord;

	int vertexAttribLoc;
	int normalAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};
