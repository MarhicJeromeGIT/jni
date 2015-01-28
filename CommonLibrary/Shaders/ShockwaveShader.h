#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// wut wut
//
//****************************************************************************

class ShockwaveShaderPrivate : public Shader
{
	friend class ShaderManager;
protected:
	ShockwaveShaderPrivate();
	~ShockwaveShaderPrivate(void);

	std::string vertexHeader;
	std::string fragmentHeader;

	UniformMat3f*	   uNormalMat;

	int vertexAttribLoc;
	int normalAttribLoc;

	virtual void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	virtual void load();
};

class ShockwaveShader : public ShockwaveShaderPrivate
{
friend class ShaderManager;
private:
	ShockwaveShader(void);
	~ShockwaveShader(void);

	virtual void load();

public:

	UniformSampler2D*  uTextureSampler;
	UniformFloat*      uTime;

};
