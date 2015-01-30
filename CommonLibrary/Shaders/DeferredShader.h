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

	int vertexAttribLoc;
	int normalAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

//****************************************************************************
// Deferred shading : Full screen pass
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

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};