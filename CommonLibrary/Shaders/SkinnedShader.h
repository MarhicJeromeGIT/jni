#pragma once

#include "Uniform.h"
#include "Shader.h"

//****************************************************************************
// Skeletal animation shaders
//
//****************************************************************************

class SkeletalAnimationShader : public Shader
{
friend class ShaderManager;
protected:
	SkeletalAnimationShader();

public:

	// location pour les attributes du skinning vertex shader
	int vertexAttribLoc;
	int	bonesIndexAttribLoc;
	int	bonesWeightAttribLoc;

	void enable( const ShaderParams& params );
	void disable();

	UniformMat4f*     uBonesMat;
};

//****************************************************************************
// Skinning shader with diffuse texture
//
//****************************************************************************

class SkinningShader : public SkeletalAnimationShader
{
public:
	std::string vertexHeader;
	std::string fragmentHeader;

	SkinningShader(void);
	~SkinningShader(void);

	virtual void Draw( Mesh* mesh );
	UniformVec4f* 	  uClipPlane;
	UniformVec4f*     uLightPos;
	UniformMat3f*     uNormalMat;
	UniformSampler2D* uTexture;

	int normalAttribLoc;
	int texCoordAttribLoc;

	virtual void enable( const ShaderParams& params );
	virtual void disable();
	virtual void load();
};


//****************************************************************************
// Skinning shader with diffuse texture and cubemap reflection
//
//****************************************************************************

class SkinningShaderCubemapReflection : public SkinningShader
{
public:

	SkinningShaderCubemapReflection(void);
	~SkinningShaderCubemapReflection(void);

	UniformSamplerCube* uCubemap;
	UniformMat4f*       uInverseViewMat;

	virtual void load();
};

//****************************************************************************
// Skinning shader with normal and specular textures
//
//****************************************************************************

class SkinningSpecularShader : public SkeletalAnimationShader
{
public:
	std::string vertexHeader;
	std::string fragmentHeader;

	SkinningSpecularShader(void);
	~SkinningSpecularShader(void);

	virtual void Draw( Mesh* mesh );
	UniformVec4f* 	  uClipPlane;
	UniformVec4f*     uLightPos;
	UniformMat3f*     uNormalMat;
	UniformSampler2D* uTexture;
	UniformSampler2D* uNormalMap;
	UniformSampler2D* uSpecularMap;

	int normalAttribLoc;
	int texCoordAttribLoc;

	void enable( const ShaderParams& params );
	void disable();
	virtual void load();
};


//****************************************************************************
// Skinning shader with normal texture + crayon map + toon shading
//
//****************************************************************************

class SkinningNormalCrayonShader : public SkinningSpecularShader
{
public:
	SkinningNormalCrayonShader(void);
	~SkinningNormalCrayonShader(void);

	UniformSampler2D* uCrayonMap;

	virtual void load();
};

//****************************************************************************
// Skinning shader with normal and specular textures, and mixed diffuse color
//
//****************************************************************************

class SkinningSpecularMixDiffuseShader : public SkinningSpecularShader
{
public:
	SkinningSpecularMixDiffuseShader(void);
	~SkinningSpecularMixDiffuseShader(void);

	UniformVec4f* 	  uColor1;
	UniformVec4f*     uColor2;

	virtual void load();
};

//****************************************************************************
// Shadow map generation for skinning shader
//
//****************************************************************************

class SkinningShadowMapShader : public SkeletalAnimationShader
{
public:
	UniformFloat* objectCol;

	virtual void Draw( Mesh* mesh );
	virtual void load();

	SkinningShadowMapShader(void);
	~SkinningShadowMapShader(void);
};
