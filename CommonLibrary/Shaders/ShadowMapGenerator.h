#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************

class ShadowMapGenerator : public Shader
{
friend class ShaderManager;

private:
	ShadowMapGenerator(void);
	~ShadowMapGenerator(void);

public:
	UniformFloat* objectCol;

	int vertexAttribLoc;
	void load();

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh );
};
