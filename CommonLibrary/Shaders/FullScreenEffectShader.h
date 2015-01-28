#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// A Sobel line detection Shader
//
//****************************************************************************
class SobelShader : public Shader
{
	friend class ShaderManager;
private:
	SobelShader(void);
	~SobelShader(void);

public:

	int vertexAttribLoc;
	int texCoordAttribLoc;

	UniformSampler2D* textureUniform;
	UniformFloat* uTexWidth;
	UniformFloat* uTexHeight;

	void load();
	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};


//****************************************************************************
// A Line Shader (draw the sobel shader results on screen) (Gaussian Blur ?)
//
//****************************************************************************


class LineShader : public Shader
{
friend class ShaderManager;
private:
	LineShader(void);
	~LineShader(void);

public:

	UniformSampler2D*  uTextureSampler;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

//****************************************************************************
// A texture mix shader
//
//****************************************************************************

class TextureMixShader : public Shader
{
friend class ShaderManager;
private:
	TextureMixShader(void);
	~TextureMixShader(void);

public:
	UniformSampler2D*  uTextureSampler1;
	UniformSampler2D*  uTextureSampler2;
	UniformBoolean*    uTextureMix;
	UniformBoolean*    uTextureMul;
	UniformBoolean*    uTextureDiff;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();

};

//****************************************************************************
// Gaussian Blur (Vertical and Horizontal pass)
//
//****************************************************************************

class GaussianBlurShader : public Shader
{
friend class ShaderManager;
private:
	GaussianBlurShader(void);
	~GaussianBlurShader(void);

public:

	UniformSampler2D*  uTextureSampler;
	UniformBoolean*    uVerticalPass;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

