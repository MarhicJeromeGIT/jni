#include "GrassShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"
using namespace glm;
#include "CommonLibrary.h"

//****************************************************************************
// Texture shader and nothing else
//
//****************************************************************************

MovingGrassShader::MovingGrassShader() : Shader()
{
}

void MovingGrassShader::load()
{
	LOGT("SHADER","MovingGrassShader::load");

	string vs = readFile_string( shader_path + string("GrassShader.vs") );
	string ps = readFile_string( shader_path + string("GrassShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uGrassHeightSampler = (UniformSampler2D*) GetUniformByName("grassHeightSampler");
	assert(uGrassHeightSampler != 0);

	uGrassFadeSampler = (UniformSampler2D*) GetUniformByName("grassFadeSampler");
	assert(uGrassFadeSampler != 0);

	uGrassShadowSampler = (UniformSampler2D*) GetUniformByName("grassShadowSampler");
	assert(uGrassShadowSampler != 0);

	uLightPosition = (UniformVec3f*) GetUniformByName("lightPosition");
	assert(uLightPosition != 0);

	uWindDirection = (UniformVec2f*) GetUniformByName("windDirection");
	assert(uWindDirection != 0);

	uTime = (UniformFloat*) GetUniformByName("time");
	assert(uTime != 0);

	uIntensity = (UniformFloat*) GetUniformByName("intensity");
	assert(uIntensity != 0);
	uIntensity->setValue(1.5);

	vertexAttribLoc     = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc   = glGetAttribLocation( getProgram(), "iTexCoord" );
	normaleAttribLoc    = glGetAttribLocation( getProgram(), "normale" );
	grassCoordAttribLoc = glGetAttribLocation( getProgram(), "grassCoord" );
}

MovingGrassShader::~MovingGrassShader(void)
{
}

void MovingGrassShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
	glEnableVertexAttribArray(normaleAttribLoc);
	glEnableVertexAttribArray(grassCoordAttribLoc);

	uLightPosition->setValue( vec3( params.lights[0].lightPosition ) );

	uTime->setValue( params.time );
}

void MovingGrassShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	glDisableVertexAttribArray(normaleAttribLoc);
	glDisableVertexAttribArray(grassCoordAttribLoc);

	Shader::disable();
}

void MovingGrassShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normaleAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer2 );
	glVertexAttribPointer( grassCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );

	if( mesh->nbTriangles < 20000 )
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
	else
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
	}
}

//****************************************************************************
// Project shadows on the grass plane
//
//****************************************************************************


GrassShadowProjectionShader::GrassShadowProjectionShader(void) : Shader()
{
}

GrassShadowProjectionShader::~GrassShadowProjectionShader(void)
{
}

void GrassShadowProjectionShader::load()
{
	LOGT("SHADER","GrassShadowProjectionShader::load");

	string vs = readFile_string( shader_path + string("GrassShadowProjection.vs") );
	string ps = readFile_string( shader_path + string("GrassShadowProjection.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	lightMap = (UniformSampler2D*) GetUniformByName("lightMap");
	assert(lightMap != 0);

	lightMat = (UniformMat4f*) GetUniformByName("LightMat");
	assert(lightMat != 0);

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
}

void GrassShadowProjectionShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
}

void GrassShadowProjectionShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);

	Shader::disable();
}

void GrassShadowProjectionShader::Draw( Mesh* m)
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );

	if( mesh->nbTriangles < 20000 )
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
	else
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
	}
}