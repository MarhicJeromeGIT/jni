#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Texture shader
//
//****************************************************************************
class WaterShader : public Shader
{
friend class ShaderManager;

private:
	WaterShader(void);
	~WaterShader(void);

public:

	int vertexAttribLoc;
	int texCoordAttribLoc;
	int normalAttribLoc;
	int tangenteAttribLoc;

	UniformSampler2D*  uNoiseSampler;
	UniformSampler2D*  uWaterTextureSampler;
	UniformSampler2D*  uTextureSampler;
	UniformMat4f*      uReflectedProjMat;
	UniformMat3f*      uNormalMat;
	UniformVec3f*      uLightPos;
	UniformFloat*	   uTime;
	UniformSampler2D*  uShadowMapSampler;
	UniformMat4f* 	   lightMapUniform;

	virtual void load();
	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};
