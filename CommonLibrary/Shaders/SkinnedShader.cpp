#include "SkinnedShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

using namespace glm;

SkeletalAnimationShader::SkeletalAnimationShader() : Shader()
{
	// location pour les attributes du skinning vertex shader
	vertexAttribLoc      = 0;
	bonesIndexAttribLoc  = 0;
	bonesWeightAttribLoc = 0;
}

void SkeletalAnimationShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(bonesIndexAttribLoc);
	glEnableVertexAttribArray(bonesWeightAttribLoc);

}

void SkeletalAnimationShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(bonesIndexAttribLoc);
	glDisableVertexAttribArray(bonesWeightAttribLoc);
	Shader::disable();
}


//*****************************************************
// SKINNING SHADER
//
//*****************************************************

SkinningShader::SkinningShader(void) : SkeletalAnimationShader()
{
}

void SkinningShader::load()
{
	LOGT("SHADER","SkinningShader::load");

	string vs = vertexHeader   + readFile_string( shader_path + string("skinningshader.vs") );
	string ps = fragmentHeader + readFile_string( shader_path + string("skinningshader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uNormalMat = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMat != 0);

	uLightPos = (UniformVec4f*) GetUniformByName("LightPos");
	assert(uLightPos != 0);

	uTexture = (UniformSampler2D*) GetUniformByName("tex");
	if( uTexture == NULL )
	{
#ifdef __ANDROID__
	LOGE("NOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
	LOGE("uTexture is NULL");
#endif
		assert(uTexture != 0);
	}

	uBonesMat = (UniformMat4f*) GetUniformByName( "uBonesMatrix[0]" ); // OU "uBonesMatrix"
	if( uBonesMat == NULL )
	{
#ifdef __ANDROID__
	LOGE("NOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
#endif
	}
	assert(uBonesMat != 0);

	uClipPlane = (UniformVec4f*) GetUniformByName("clipPlane");
	assert( uClipPlane != 0 );

	vertexAttribLoc      = glGetAttribLocation( getProgram(), "vertexPosition" );
	bonesIndexAttribLoc  = glGetAttribLocation( getProgram(), "bonesIndex" );
	bonesWeightAttribLoc = glGetAttribLocation( getProgram(), "bonesWeight" );
	normalAttribLoc      = glGetAttribLocation( getProgram(), "vertexNormal" );
	texCoordAttribLoc    = glGetAttribLocation( getProgram(), "vertexTexCoord" );
}

SkinningShader::~SkinningShader(void)
{
}

void SkinningShader::Draw( Mesh* m )
{
#ifndef NDEBUG
	GLenum errCode;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SkinningShader::Draw %d",errCode);
	}
#endif

	InternalMesh* mesh = (InternalMesh*) m;

	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->bonesIndexBuffer ); 	// /!\ attribIpointer parce qu'on passe des indices (int) !
	glVertexAttribPointer(bonesIndexAttribLoc, 4, GL_FLOAT, GL_FALSE,0,0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->bonesWeightBuffer );
	glVertexAttribPointer( bonesWeightAttribLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0,0 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );

#ifndef NDEBUG
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error MainMenuGameState::Draw glDrawElements %d",errCode);
	}
#endif
}

void SkinningShader::enable( const ShaderParams& params )
{
	SkeletalAnimationShader::enable(params);
	glEnableVertexAttribArray(normalAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
	uClipPlane->setValue( params.clipPlane );

	uLightPos->setValue( params.lights[0].lightPosition );

	//the correct matrix to transform the normal is the transpose of the inverse of the (upper left of) M matrix
	glm::mat3 upperTopMV = glm::mat3(  params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMat->setValue( normalMatrix );
}

void SkinningShader::disable()
{
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	SkeletalAnimationShader::disable();
}

//****************************************************************************
// Skinning shader with diffuse texture and cubemap reflection
//
//****************************************************************************

SkinningShaderCubemapReflection::SkinningShaderCubemapReflection() : SkinningShader()
{
}

SkinningShaderCubemapReflection::~SkinningShaderCubemapReflection()
{
}

void SkinningShaderCubemapReflection::load()
{
	LOGT("SHADER","SkinningShaderCubemapReflection::load");

	vertexHeader   = "#define USE_CUBEMAP \n";
	fragmentHeader = "#define USE_CUBEMAP \n";

	SkinningShader::load();

	uCubemap = (UniformSamplerCube*)GetUniformByName("gCubemapTexture");
	assert(uCubemap!= NULL);

	uInverseViewMat = (UniformMat4f*)GetUniformByName("invviewmat");
	assert(uInverseViewMat != NULL);
}

//****************************************************************************
// Skinning shader with normal and specular textures
//
//****************************************************************************

SkinningSpecularShader::SkinningSpecularShader(void) : SkeletalAnimationShader()
{
	fragmentHeader = "#define SPECULAR_MAP \n";
}

void SkinningSpecularShader::load()
{
	LOGT("SHADER","SkinningSpecularShader::load");

	string vs = vertexHeader   + readFile_string( shader_path + string("SkinningSpecularShader.vs") );
	string ps = fragmentHeader + readFile_string( shader_path + string("SkinningSpecularShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uNormalMat = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMat != 0);

	uLightPos = (UniformVec4f*) GetUniformByName("LightPos");
	assert(uLightPos != 0);

	uTexture = (UniformSampler2D*) GetUniformByName("colorMap");
	assert(uTexture != 0);

	uNormalMap = (UniformSampler2D*) GetUniformByName("normalMap");
	assert(uNormalMap != 0);

	uSpecularMap = (UniformSampler2D*) GetUniformByName("specularMap");
	assert(uSpecularMap != 0);

	uBonesMat = (UniformMat4f*) GetUniformByName( "uBonesMatrix[0]" ); // OU "uBonesMatrix"
	assert(uBonesMat != 0);

	uClipPlane = (UniformVec4f*) GetUniformByName("clipPlane");
	assert( uClipPlane != 0 );

	vertexAttribLoc      = glGetAttribLocation( getProgram(), "vertexPosition" );
	bonesIndexAttribLoc  = glGetAttribLocation( getProgram(), "bonesIndex" );
	bonesWeightAttribLoc = glGetAttribLocation( getProgram(), "bonesWeight" );
	normalAttribLoc      = glGetAttribLocation( getProgram(), "vertexNormal" );
	texCoordAttribLoc    = glGetAttribLocation( getProgram(), "vertexTexCoord" );
}

SkinningSpecularShader::~SkinningSpecularShader(void)
{
}

void SkinningSpecularShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;

	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->bonesIndexBuffer ); 	// /!\ attribIpointer parce qu'on passe des indices (int) !

	glVertexAttribPointer(bonesIndexAttribLoc, 4, GL_FLOAT, GL_FALSE,0,0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->bonesWeightBuffer );
	glVertexAttribPointer( bonesWeightAttribLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0,0 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}

void SkinningSpecularShader::enable( const ShaderParams& params )
{
	SkeletalAnimationShader::enable(params);
	glEnableVertexAttribArray(normalAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
	uClipPlane->setValue( params.clipPlane );

	vec4 lightDir =  vec4( vec3(params.lights[0].lightPosition), 0.0 );
	uLightPos->setValue( normalize( params.viewMatrix * lightDir ) );

	//the correct matrix to transform the normal is the transpose of the inverse of the (upper left of) M matrix
	glm::mat3 upperTopMV = glm::mat3(  params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMat->setValue( normalMatrix );
}

void SkinningSpecularShader::disable()
{
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	SkeletalAnimationShader::disable();
}


//****************************************************************************
// Skinning shader with normal texture + crayon map + toon shading
//
//****************************************************************************


SkinningNormalCrayonShader::SkinningNormalCrayonShader(void) : SkinningSpecularShader()
{
	vertexHeader   = "#define CRAYON_STYLE \n";
	fragmentHeader = "#define SPECULAR_MAP \n #define CRAYON_STYLE \n";
}

SkinningNormalCrayonShader::~SkinningNormalCrayonShader(void)
{
}

void SkinningNormalCrayonShader::load()
{
	LOGT("SHADER","SkinningNormalCrayonShader::load");

	SkinningSpecularShader::load();

	uCrayonMap = (UniformSampler2D*)GetUniformByName("crayonMap");
	assert(uCrayonMap != 0);
}


//****************************************************************************
// Skinning shader with normal and specular textures, and mixed diffuse color
//
//****************************************************************************

SkinningSpecularMixDiffuseShader::SkinningSpecularMixDiffuseShader(void) : SkinningSpecularShader()
{
	fragmentHeader = "#define SPECULAR_MAP \n #define DIFFUSE_MIX \n";
}

SkinningSpecularMixDiffuseShader::~SkinningSpecularMixDiffuseShader(void)
{
}

void SkinningSpecularMixDiffuseShader::load()
{
	LOGT("SHADER","SkinningSpecularMixDiffuseShader::load");

	SkinningSpecularShader::load();

	uColor1 = (UniformVec4f*) GetUniformByName("color1");
	assert(uColor1 != 0);

	uColor2 = (UniformVec4f*) GetUniformByName("color2");
	assert(uColor2 != 0);
}

//*****************************************************
// SKINNED SHADOW MAP GENERATOR SHADER
//
//*****************************************************

SkinningShadowMapShader::SkinningShadowMapShader(void) : SkeletalAnimationShader()
{
}

void SkinningShadowMapShader::load()
{
	LOGT("SHADER","SkinningShadowMapShader::load");

	string vs = readFile_string( shader_path + string("shadowMapSkinningGenerator.vs") );
	string ps = readFile_string( shader_path + string("shadowMapGenerator.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uBonesMat = (UniformMat4f*) GetUniformByName( "uBonesMatrix[0]" ); // OU "uBonesMatrix"
	assert(uBonesMat != 0);

	objectCol = (UniformFloat*) GetUniformByName("objectCol");
	if( objectCol == NULL )
	{
		objectCol = new UniformFloat( -1, this );
	}
	objectCol->setValue( 0.0f );

	vertexAttribLoc      = glGetAttribLocation( getProgram(), "vertexPosition" );
	bonesIndexAttribLoc  = glGetAttribLocation( getProgram(), "bonesIndex" );
	bonesWeightAttribLoc = glGetAttribLocation( getProgram(), "bonesWeight" );
}

SkinningShadowMapShader::~SkinningShadowMapShader(void)
{
}

void SkinningShadowMapShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->bonesIndexBuffer ); 	// /!\ attribIpointer parce qu'on passe des indices (int) !

	glVertexAttribPointer(bonesIndexAttribLoc, 4, GL_FLOAT, false,0,0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->bonesWeightBuffer );
	glVertexAttribPointer( bonesWeightAttribLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}

