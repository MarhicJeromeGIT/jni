#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Texture shader and nothing else
//
//****************************************************************************

class SimpleTextureShader : public Shader
{
friend class ShaderManager;

protected:
	SimpleTextureShader(void);
	~SimpleTextureShader(void);

public:
	std::string vertexHeader;
	std::string fragmentHeader;

	UniformSampler2D*  uTextureSampler;
	UniformVec4f*      uUVScaleAndOffset;
	UniformVec4f*      color;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	virtual void enable( const ShaderParams& params );
	virtual void disable();
	virtual void Draw( Mesh* mesh);
	virtual void load();
};

//****************************************************************************
// TextureShader with Per-vertex transparency
//
//****************************************************************************

class PerVertexTransparencyTextureShader : public SimpleTextureShader
{
friend class ShaderManager;
private:
	PerVertexTransparencyTextureShader(void);
	~PerVertexTransparencyTextureShader(void);

public:
	int vertexTransparencyAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

//****************************************************************************
// Texture shader with lighting stuff
//
//****************************************************************************
class TextureShader : public Shader
{
friend class ShaderManager;
private:
	TextureShader(void);
	~TextureShader(void);
	void load();

public:

	UniformSampler2D*  uTextureSampler;
	UniformSampler2D*  uShadowMapSampler;
	UniformMat4f* 	   lightMapUniform;
	UniformVec4f* 	   uClipPlane;
	UniformMat3f*	   uNormalMat;
	UniformVec3f*	   uLightPos;
	UniformVec4f*	   uAmbientColor;
	UniformBoolean*    uUseScreenCoord;

	int vertexAttribLoc;
	int texCoordAttribLoc;
	int normalAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
};

//****************************************************************************
// Multi Texture shader
//
//****************************************************************************
#ifndef __ANDROID__ 

class MultiTextureShader : public Shader
{
friend class ShaderManager;

private:
	MultiTextureShader(int nbTextures);
	~MultiTextureShader(void);
public:

	unsigned int        numTextures;
	UniformSampler2DArray*  uTextureArraySampler;
	UniformSampler2D**    uTextureSampler;
	UniformSampler2D*   uShadowMapSampler;
	UniformMat4f* 	    lightMapUniform;
	UniformVec4f* 	    uClipPlane;
	UniformMat3f*	    uNormalMat;
	UniformVec3f*	    uLightPos;
	UniformVec4f*	    uAmbientColor;

	int vertexAttribLoc;
	int* texCoordAttribLoc;
	int normalAttribLoc;
	int textureWeightLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

//****************************************************************************
// Texture Array shader
//
//****************************************************************************
class TextureArrayShader : public Shader
{
friend class ShaderManager;
private:
	TextureArrayShader(void);
	~TextureArrayShader(void);

public:

	UniformSampler2DArray*  uTextureSampler;
	UniformVec4f*	   		uDiffuseColor;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	void Draw( Mesh* mesh);
	void load();
};

//****************************************************************************
// Display a slice of a 3D texture
//
//****************************************************************************

class TextureSliceShader : public Shader
{
friend class ShaderManager;

protected:
	TextureSliceShader(void);
	~TextureSliceShader(void);

public:
	std::string vertexHeader;
	std::string fragmentHeader;

	UniformSampler3D*  uTextureSampler;

	int vertexAttribLoc;
	int texCoordAttribLoc;

	virtual void enable( const ShaderParams& params );
	virtual void disable();
	virtual void Draw( Mesh* mesh);
	virtual void load();
};

#endif