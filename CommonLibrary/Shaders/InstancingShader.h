#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// A static object instancing shader
//
//****************************************************************************
class InstancingShader : public Shader
{
	friend class ShaderManager;
private:
	InstancingShader(void);
	~InstancingShader(void);
public:
	UniformVec4f*  uDiffuseColor;

	int vertexAttribLoc;

	// Instancing data
	int modelMatrixLoc;

	void load();
	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};
