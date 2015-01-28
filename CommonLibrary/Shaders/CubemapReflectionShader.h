#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Cubemap reflection shader
//
//****************************************************************************
class CubemapReflectionShader : public Shader
{
	friend class ShaderManager;
protected:
	CubemapReflectionShader(void);
	~CubemapReflectionShader(void);
public:
	std::string vertexHeader;
	std::string fragmentHeader;

	UniformSamplerCube* uCubeMapSampler;

	UniformMat3f*  uNormalMatrix;
	UniformVec4f*  uLightPosition;
	UniformMat4f*  uInvViewMat;

	int vertexAttribLoc;
	int normalAttribLoc;

	virtual void enable( const ShaderParams& params );
	virtual void disable();
	virtual void Draw( Mesh* mesh);
	virtual void load();
};

//****************************************************************************
// Cubemap reflection shader with diffuse texture
//
//****************************************************************************

class CubemapReflectionShaderWithTexture : public CubemapReflectionShader
{
	friend class ShaderManager;
protected:
	CubemapReflectionShaderWithTexture(void);
	~CubemapReflectionShaderWithTexture(void);
public:
	UniformSampler2D* uDiffuseSampler;

	int texcoordAttribLoc;

	virtual void enable( const ShaderParams& params );
	virtual void disable();
	virtual void Draw( Mesh* mesh);
	virtual void load();
};

//****************************************************************************
// Cubemap reflection shader with diffuse texture and normal
//
//****************************************************************************

class CubemapReflectionShaderWithTextureAndNormal : public CubemapReflectionShaderWithTexture
{
	friend class ShaderManager;
protected:
	CubemapReflectionShaderWithTextureAndNormal(void);
	~CubemapReflectionShaderWithTextureAndNormal(void);
public:
	UniformSampler2D* uNormalMapSampler;

	int tangenteAttribLoc;

	virtual void load();
	virtual void enable( const ShaderParams& params );
	virtual void disable();
	virtual void Draw( Mesh* mesh);

};
