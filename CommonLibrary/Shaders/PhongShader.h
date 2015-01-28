#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Phong shader
//
//****************************************************************************
class PhongShader : public Shader
{
	friend class ShaderManager;
private:
	PhongShader(void);
	~PhongShader(void);

public:

	UniformMat3f*  uNormalMatrix;
	UniformVec4f*  uDiffuseColor;
	UniformVec4f*  uLightPosition;
	UniformVec4f*  uLightDiffuseColor;
	UniformVec4f*  uAmbientColor;

	int vertexAttribLoc;
	int normalAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

//****************************************************************************
// Phong shader with texture
//
//****************************************************************************
class PhongTextureShader : public Shader
{
	friend class ShaderManager;
private:
	PhongTextureShader(void);
	~PhongTextureShader(void);

public:

	UniformMat3f*  uNormalMatrix;
	UniformVec4f*  uDiffuseColor;
	UniformVec4f*  uLightPosition;
	UniformVec4f*  uLightDiffuseColor;
	UniformVec4f*  uAmbientColor;
	UniformSampler2D*  uTextureSampler;

	int vertexAttribLoc;
	int normalAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};


//****************************************************************************
// Phong shader with depth texture
//
//****************************************************************************
class PhongWithDepthShader : public Shader
{
	friend class ShaderManager;
private:
	PhongWithDepthShader(void);
	~PhongWithDepthShader(void);

public:

	UniformMat3f*  uNormalMatrix;
	UniformVec4f*  uDiffuseColor;
	UniformVec4f*  uLightPosition;
	UniformVec4f*  uLightDiffuseColor;
	UniformVec4f*  uAmbientColor;
	UniformSampler2D*  uTextureSampler;
	UniformFloat*  resX;
	UniformFloat*  resY;
	UniformFloat*  uIntensity;
	UniformFloat*  uSigma;
	int vertexAttribLoc;
	int normalAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};
