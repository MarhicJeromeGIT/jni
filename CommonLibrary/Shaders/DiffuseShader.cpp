#include "DiffuseShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************

DiffuseShader::DiffuseShader() : Shader()
{
}

void DiffuseShader::load()
{
	LOGT("SHADER","DiffuseShader::load");

	string vs = readFile_string( shader_path + string("DiffuseShader.vs") );
	string ps = readFile_string( shader_path + string("DiffuseShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uDiffuseColor = (UniformVec4f*) GetUniformByName("color");
	assert(uDiffuseColor != 0);

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
}

DiffuseShader::~DiffuseShader(void)
{

}

void DiffuseShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
}

void DiffuseShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	Shader::disable();
}

void DiffuseShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;

	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}


//****************************************************************************
// A diffuse color shader with shadows
//
//****************************************************************************


DiffuseShadowShader::DiffuseShadowShader() : Shader()
{
	string vs = readFile( "DiffuseShadowShader.vs" );
	string ps = readFile( "DiffuseShadowShader.ps" );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	shadowMapUniform = (UniformSampler2D*) GetUniformByName( "shadowmap" );
	assert( shadowMapUniform != 0 );

	lightMapUniform = (UniformMat4f*) GetUniformByName( "LightMat" );
	assert( lightMapUniform != 0 );

	uDiffuseColor = (UniformVec4f*) GetUniformByName("color");
	assert(uDiffuseColor != 0);

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

DiffuseShadowShader::~DiffuseShadowShader(void)
{

}

void DiffuseShadowShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	lightMapUniform->setValue( params.lightMap );
	shadowMapUniform->setValue( params.shadowmap );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void DiffuseShadowShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void DiffuseShadowShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}
