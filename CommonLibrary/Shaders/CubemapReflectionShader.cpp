#include "CubemapReflectionShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

//****************************************************************************
// Cubemap reflection shader
//
//****************************************************************************

CubemapReflectionShader::CubemapReflectionShader() : Shader()
{
}

CubemapReflectionShader::~CubemapReflectionShader(void)
{

}

void CubemapReflectionShader::load()
{
	LOGT("SHADER","CubemapReflectionShader::load");

	string vs = vertexHeader   + readFile_string( shader_path + string("CubemapReflectionShader.vs") );
	string ps = fragmentHeader + readFile_string( shader_path + string("CubemapReflectionShader.ps") );
	assert( vs.size() > 0 && ps.size() > 0 );
	LOGT("SHADER","CubemapReflectionShader::load prepare to compile");
	LoadShaderFromFile(vs,ps);
	LOGT("SHADER","CubemapReflectionShader::load compiled");

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uNormalMatrix = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMatrix != 0);

	uCubeMapSampler = (UniformSamplerCube*) GetUniformByName("gCubemapTexture");
	assert(uCubeMapSampler);

	uLightPosition = (UniformVec4f*) GetUniformByName("lightPosition");
	assert(uLightPosition != 0);

	uInvViewMat = (UniformMat4f*) GetUniformByName("invviewmat");
	assert(uInvViewMat != 0 );

	vertexAttribLoc      = glGetAttribLocation( getProgram(), "vertexPosition" );
	normalAttribLoc      = glGetAttribLocation( getProgram(), "normalOrientation" );

	LOGT("SHADER","CubemapReflectionShader::load done");
}

void CubemapReflectionShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMatrix->setValue( normalMatrix );

	uLightPosition->setValue( params.viewMatrix * params.lights[0].lightPosition );

	uInvViewMat->setValue( glm::inverse(params.viewMatrix) );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);

}

void CubemapReflectionShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	Shader::disable();
}

void CubemapReflectionShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;

	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	if( mesh->nbTriangles < 20000 )
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
	else
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
	}

#ifndef NDEBUG
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		LOGE("CubemapReflectionShader::Draw %d",errCode);
		assert(false);
	}
#endif
}

//****************************************************************************
// Cubemap reflection shader with diffuse texture
//
//****************************************************************************

CubemapReflectionShaderWithTexture::CubemapReflectionShaderWithTexture() : CubemapReflectionShader()
{
	vertexHeader   = "#define USE_TEXTURE \n";
	fragmentHeader = "#define USE_TEXTURE \n";
}

CubemapReflectionShaderWithTexture::~CubemapReflectionShaderWithTexture(void)
{
}

void CubemapReflectionShaderWithTexture::load()
{
	CubemapReflectionShader::load();

	uDiffuseSampler = (UniformSampler2D*) GetUniformByName("texDiffuse");
	assert(uDiffuseSampler);

	texcoordAttribLoc = glGetAttribLocation( getProgram(), "texCoord" );
}

void CubemapReflectionShaderWithTexture::enable( const ShaderParams& params )
{
	CubemapReflectionShader::enable(params);
	glEnableVertexAttribArray(texcoordAttribLoc);
}

void CubemapReflectionShaderWithTexture::disable()
{
	glDisableVertexAttribArray(texcoordAttribLoc);
	CubemapReflectionShader::disable();
}

void CubemapReflectionShaderWithTexture::Draw( Mesh* m)
{
	InternalMesh* mesh = (InternalMesh*) m;

	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texcoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	if( mesh->nbTriangles < 20000 )
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
	else
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
	}

#ifndef NDEBUG
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		LOGE("CubemapReflectionShader::Draw %d",errCode);
		assert(false);
	}
#endif
}


//****************************************************************************
// Cubemap reflection shader with diffuse texture
//
//****************************************************************************

CubemapReflectionShaderWithTextureAndNormal::CubemapReflectionShaderWithTextureAndNormal() : CubemapReflectionShaderWithTexture()
{
	vertexHeader   = "#define USE_TEXTURE \n #define USE_NORMALMAP \n";
	fragmentHeader = "#define USE_TEXTURE \n #define USE_NORMALMAP \n";
}

CubemapReflectionShaderWithTextureAndNormal::~CubemapReflectionShaderWithTextureAndNormal(void)
{
}

void CubemapReflectionShaderWithTextureAndNormal::load()
{
	CubemapReflectionShaderWithTexture::load();

	uNormalMapSampler = (UniformSampler2D*) GetUniformByName("normalMap");
	assert(uNormalMapSampler != NULL);

	tangenteAttribLoc = glGetAttribLocation( getProgram(), "tangente" );
}

void CubemapReflectionShaderWithTextureAndNormal::enable( const ShaderParams& params )
{
	CubemapReflectionShaderWithTexture::enable(params);
	glEnableVertexAttribArray(tangenteAttribLoc);
}

void CubemapReflectionShaderWithTextureAndNormal::disable()
{
	glDisableVertexAttribArray(tangenteAttribLoc);
	CubemapReflectionShaderWithTexture::disable();
}

void CubemapReflectionShaderWithTextureAndNormal::Draw( Mesh* m)
{
	InternalMesh* mesh = (InternalMesh*) m;

	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->tangenteBuffer );
	glVertexAttribPointer( tangenteAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texcoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	if( mesh->nbTriangles < 20000 )
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
	else
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
	}

#ifndef NDEBUG
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		LOGE("CubemapReflectionShader::Draw %d",errCode);
		assert(false);
	}
#endif
}
