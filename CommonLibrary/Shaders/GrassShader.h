#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Moving Grass Shader
//
//****************************************************************************

class MovingGrassShader : public Shader
{
friend class ShaderManager;

protected:
	MovingGrassShader(void);
	~MovingGrassShader(void);

public:
	UniformSampler2D*  uTextureSampler;
	UniformSampler2D*  uGrassHeightSampler;
	UniformSampler2D*  uGrassFadeSampler;
	UniformSampler2D*  uGrassShadowSampler;

	UniformVec3f*      uLightPosition;
	UniformVec2f*      uWindDirection;
	UniformFloat*      uTime;
	UniformFloat*      uIntensity;

	int vertexAttribLoc;
	int texCoordAttribLoc;
	int normaleAttribLoc;
	int grassCoordAttribLoc;

	virtual void enable( const ShaderParams& params );
	virtual void disable();
	virtual void Draw( Mesh* mesh);
	virtual void load();
};

//****************************************************************************
// Project shadows on the grass plane
//
//****************************************************************************

class GrassShadowProjectionShader : public Shader
{
friend class ShaderManager;

protected:
	GrassShadowProjectionShader(void);
	~GrassShadowProjectionShader(void);

public:
	UniformMat4f* lightMat;
	UniformSampler2D* lightMap;

	int vertexAttribLoc;

	virtual void enable( const ShaderParams& params );
	virtual void disable();
	virtual void Draw( Mesh* mesh);
	virtual void load();

};



