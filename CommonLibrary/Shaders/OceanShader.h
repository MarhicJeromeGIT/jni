#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Ocean Shader
//
//****************************************************************************
class OceanShader : public Shader
{
	friend class ShaderManager;
private:
	OceanShader(void);
	~OceanShader(void);
public:
	UniformMat3f*  uNormalMatrix;
	UniformVec4f*  uDiffuseColor;
	UniformVec4f*  uLightPosition;
	UniformVec4f*  uLightDiffuseColor;
	UniformVec4f*  uAmbientColor;
	UniformSampler2D*  uTextureSampler;
	UniformSamplerCube*  uCubemapSampler;
	UniformMat4f*  uInvViewMat;
	UniformSampler2D* uHeightMapSampler;
	UniformSampler2D* uNormalMapSampler;
	UniformFloat*  lambda;

	int vertexAttribLoc;
	int normalAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

//****************************************************************************
// Ocean Shader : Geometry generation
//
//****************************************************************************
class OceanGeometryGeneratorShader : public Shader
{
public:
	UniformFloat* N; // size of the grid
	UniformSampler2D*  h0tilde;
	UniformSampler2D*  h0tildeConj;
	UniformFloat* time;
	UniformFloat* L;
	UniformFloat* multiplicateur;

	OceanGeometryGeneratorShader(void);
	~OceanGeometryGeneratorShader(void);

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();

};