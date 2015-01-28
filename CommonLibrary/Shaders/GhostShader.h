#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// A textured, blueish transparent shader
//
//****************************************************************************

class GhostShader : public Shader
{
friend class ShaderManager;
private:
	GhostShader(void);
	~GhostShader(void);

public:

	UniformSampler2D*  uTextureSampler;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

