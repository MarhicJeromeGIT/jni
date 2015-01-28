#include "LaserShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

LaserShader::LaserShader() : Shader()
{
}

void LaserShader::load()
{
	LOGT("SHADER","LaserShader::load");

	string vs = readFile_string( shader_path + string("LaserShader.vs") );
	string ps = readFile_string( shader_path + string("LaserShader.ps") );
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

LaserShader::~LaserShader(void)
{

}

void LaserShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	if( uTimeUniform != NULL )
	{
		uTimeUniform->setValue( params.time );
	}

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void LaserShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void LaserShader::Draw( Mesh* m )
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

//****************************************************************************
// Laser Shader with dummy vertex shader
//
//****************************************************************************

LaserShaderDummyVertex::LaserShaderDummyVertex() : Shader()
{
}

void LaserShaderDummyVertex::load()
{
	string vs = readFile_string( shader_path + string("dummy_shader.vs") );
	string ps = readFile_string( shader_path + string("LaserShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTimeUniform = (UniformFloat*) GetUniformByName("iGlobalTime");

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

LaserShaderDummyVertex::~LaserShaderDummyVertex(void)
{

}

void LaserShaderDummyVertex::enable( const ShaderParams& params )
{
	Shader::enable( params );

	if( uTimeUniform != NULL )
	{
		uTimeUniform->setValue( params.time );
	}

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void LaserShaderDummyVertex::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void LaserShaderDummyVertex::Draw( Mesh* m )
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
