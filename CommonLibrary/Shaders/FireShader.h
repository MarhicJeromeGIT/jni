#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Fire Shader
//
//****************************************************************************
class FireShader : public Shader
{
	friend class ShaderManager;
private:
	FireShader(void);
	~FireShader(void);
public:
	UniformMat3f*      uNormalMat;
	UniformSampler2D*  uDistortionSampler;
	UniformSampler2D*  uOpacitySampler;
	UniformSampler2D*  uFlameSampler;
	UniformFloat*	   uTimeUniform;

	int vertexAttribLoc;
	int texCoordAttribLoc;
	int normalAttribLoc;

	void load();
	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};
