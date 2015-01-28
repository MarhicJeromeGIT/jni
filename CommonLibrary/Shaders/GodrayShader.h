#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Light shaft shader
//
//****************************************************************************
class GodrayShader : public Shader
{
	friend class ShaderManager;
public:
	GodrayShader(void);
	~GodrayShader(void);

public:
	UniformSampler2D*  uTextureSampler;
	UniformVec2f*      uLightposScreen;
	UniformFloat*      uWeight;
	UniformFloat*      uDecay;
	UniformFloat*      uExposure;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	virtual void load();

};
