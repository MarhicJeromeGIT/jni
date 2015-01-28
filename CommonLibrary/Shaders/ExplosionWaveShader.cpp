#include "ExplosionWaveShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

//****************************************************************************
// Desired effect : http://www.youtube.com/watch?v=1a6vKnwXzC4
//
//****************************************************************************

ExplosionWaveShader::ExplosionWaveShader() : Shader()
{
}

void ExplosionWaveShader::load()
{
	LOGT("SHADER","ExplosionWaveShader::load");

	string vs = readFile( string( shader_path + string("ExplosionWaveShader.vs")).c_str() );
	string ps = readFile( string( shader_path + string("ExplosionWaveShader.ps")).c_str() );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "texCoord" );
}

ExplosionWaveShader::~ExplosionWaveShader(void)
{

}

void ExplosionWaveShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void ExplosionWaveShader::disable()
{
	glDisableVertexAttribArray(texCoordAttribLoc);
	glDisableVertexAttribArray(vertexAttribLoc);
	Shader::disable();
}

void ExplosionWaveShader::Draw( Mesh* m )
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

