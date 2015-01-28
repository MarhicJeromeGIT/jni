#include "FlameThrowerShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

FlameThrowerShader::FlameThrowerShader() : Shader()
{
}

void FlameThrowerShader::load()
{
	string vs = readFile_string( shader_path + string("FlameThrower.vs") );
	string ps = readFile_string( shader_path + string("FlameThrower.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTimeUniform = (UniformFloat*) GetUniformByName("iGlobalTime");
	//assert(uTimeUniform != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

FlameThrowerShader::~FlameThrowerShader(void)
{

}

void FlameThrowerShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	if( uTimeUniform != NULL )
	{
		uTimeUniform->setValue( params.time );
	}

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void FlameThrowerShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void FlameThrowerShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
}

//****************************************************************************
// FlameThrowerShader Shader with dummy vertex shader
//
//****************************************************************************

FlameThrowerShaderSimple::FlameThrowerShaderSimple() : Shader()
{
}

void FlameThrowerShaderSimple::load()
{
	string vs = readFile_string( shader_path + string("dummy_shader.vs") );
	string ps = readFile_string( shader_path + string("FlameThrower.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTimeUniform = (UniformFloat*) GetUniformByName("iGlobalTime");
	//assert(uTimeUniform != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

FlameThrowerShaderSimple::~FlameThrowerShaderSimple(void)
{

}

void FlameThrowerShaderSimple::enable( const ShaderParams& params )
{
	Shader::enable( params );

	if( uTimeUniform != NULL )
	{
		uTimeUniform->setValue( params.time );
	}

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void FlameThrowerShaderSimple::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void FlameThrowerShaderSimple::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
}
