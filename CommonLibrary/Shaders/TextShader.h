#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// a colored text
//
//****************************************************************************

class ColorTextShader : public Shader
{
friend class ShaderManager;
private:
	ColorTextShader(void);
	~ColorTextShader(void);

public:

	UniformSampler2D*  uTextureSampler;
	UniformVec4f*      uUVScaleAndOffset;

	int vertexAttribLoc;
	int texCoordAttribLoc;
	int colorAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};


