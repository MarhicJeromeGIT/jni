#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************
class TerrainTesselationShader : public Shader
{
	friend class ShaderManager;
public:
	TerrainTesselationShader(void);
	~TerrainTesselationShader(void);
public:
	UniformVec4f*     uDiffuseColor;
	UniformSampler2D* uHeightmap;
	UniformFloat*     uIntensity; // height of the heightmap

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void load();
	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};
