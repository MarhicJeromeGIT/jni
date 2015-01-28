#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
//
//
//****************************************************************************


#ifndef __ANDROID__

class StaticMeshShader : public Shader
{
public:	
	StaticMeshShader(void);
	~StaticMeshShader(void);

	int vertexAttribLoc;
	int texCoordAttribLoc;
	virtual void enable( const ShaderParams& params );
	virtual void disable();
};

class BlenderShader : public StaticMeshShader
{
public:
	BlenderShader();
	~BlenderShader();

	virtual void Draw( Mesh* mesh );

	UniformVec4f *     uAmbientColor;
	UniformVec4f *     uDiffuseColor;
	UniformVec4f *     uSpecularColor;
	UniformSampler2D*  uTextureSampler; // same texture for ambiant and diffuse
	UniformBoolean*    uUseTexture;

	UniformMat3f *     uNormalMat;
	UniformVec3f*	   uLightPos;
	int				   normalAttribLoc;
	UniformVec4f*      uClipPlane;
	UniformMat4f*	   uClipMat;
	
	void load();
	void enable( const ShaderParams& params );
	void disable();
};

class SimpleStaticMeshShader : public  StaticMeshShader
{
public:
	SimpleStaticMeshShader(void);
	~SimpleStaticMeshShader(void);

	virtual void Draw( Mesh* mesh );

	UniformSampler2D*  uTextureSampler;
};

class LightStaticMeshShader : public  StaticMeshShader
{
public:
	LightStaticMeshShader(void);
	~LightStaticMeshShader(void);

	virtual void Draw( Mesh* mesh );

	UniformMat3f *     uNormalMat;
	UniformSampler2D*  uTextureSampler;
	UniformVec3f*	   uLightPos;
	int				   normalAttribLoc;
	UniformVec4f*      uClipPlane;
	UniformMat4f*	   uClipMat;

	void enable( const ShaderParams& params );
	void disable();
};


class StaticMeshShadowMapShader : public StaticMeshShader
{
public:
	StaticMeshShadowMapShader(void);
	~StaticMeshShadowMapShader(void);

	virtual void Draw( Mesh* mesh );

	UniformSampler2D* uShadowMapSampler;
	UniformSampler2D* uTextureSampler;
	UniformMat4f*     uLightMat;
};
#endif


