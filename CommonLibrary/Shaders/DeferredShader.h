#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Deferred pass : Write useful data to the G-buffer
//
//****************************************************************************
class DeferredPassShader : public Shader
{
	friend class ShaderManager;
private:
	DeferredPassShader(void);
	~DeferredPassShader(void);

public:
	UniformMat3f*  uNormalMatrix;
	UniformSampler2D* uTextureSampler;

	int vertexAttribLoc;
	int normalAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

//****************************************************************************
// Deferred shading : Lighting pass
//
//****************************************************************************
class DeferredShaderFullScreen : public Shader
{
	friend class ShaderManager;
private:
	DeferredShaderFullScreen(void);
	~DeferredShaderFullScreen(void);

public:
	UniformSampler2D* uNormalMap;
	UniformSampler2D* uVertexMap;
	UniformVec4f*     uLightPosition;
	UniformVec4f*     uLightColor;
	UniformFloat*     uLightAttenuation;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

//****************************************************************************
// Deferred shading : Full Screen combine pass
//
//****************************************************************************
class DeferredShaderLightCombine : public Shader
{
	friend class ShaderManager;
private:
	DeferredShaderLightCombine(void);
	~DeferredShaderLightCombine(void);

public:
	UniformSampler2D* uColorMap;
	UniformSampler2D* uLightMap;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

