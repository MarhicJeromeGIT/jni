#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************
class ExplosionWaveShader : public Shader
{
	friend class ShaderManager;
private:
	ExplosionWaveShader(void);
	~ExplosionWaveShader(void);
public:

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void load();
	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};
