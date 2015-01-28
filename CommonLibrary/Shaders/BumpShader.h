#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Bump mapping shader
//
//****************************************************************************
class BumpShader : public Shader
{
	friend class ShaderManager;
protected:
	BumpShader(void);
	~BumpShader(void);

public:
	UniformSampler2D*  uTextureSampler;
	UniformSampler2D*  uShadowMapSampler;
	UniformSampler2D*  uNormalMap;
	UniformMat4f* 	   lightMatUniform;
	UniformVec4f* 	   uClipPlane;
	UniformMat3f*	   uNormalMat;
	UniformBoolean*    uOutline;

	// Lighting :
	UniformVec3f*	   uLightPos;
	UniformVec3f*      uLightAmbient;
	UniformVec3f*	   uLightDiffuse;
	UniformVec3f*	   uLightSpecular;
	UniformFloat*	   uShininess;

	int vertexAttribLoc;
	int texCoordAttribLoc;
	int normalAttribLoc;
	int tangenteAttribLoc;
	int bitangenteAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	virtual void load();

protected:
	std::string fragmentHeader;
};

//****************************************************************************
// Bump mapping shader with specular map
//
//****************************************************************************
class BumpSpecularMapShader : public BumpShader
{
	friend class ShaderManager;
protected:
	BumpSpecularMapShader(void);
	~BumpSpecularMapShader(void);

public:
	UniformSampler2D*  uSpecularMap;
	UniformVec4f*      uDiffuseMult;

	virtual void load();
};

//****************************************************************************
// Bump mapping shader with specular map and mixed color diffuse value
//
//****************************************************************************
class BumpMixSpecularMapShader : public BumpSpecularMapShader
{
	friend class ShaderManager;
protected:
	BumpMixSpecularMapShader(void);
	~BumpMixSpecularMapShader(void);

public:
	UniformVec4f*      uDiffuseMult2;

	virtual void load();
};
