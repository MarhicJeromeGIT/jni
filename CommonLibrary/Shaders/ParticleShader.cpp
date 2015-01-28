#include "ParticleShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

//****************************************************************************
// A Particle Shader
//
//****************************************************************************

ParticleShader::ParticleShader() : Shader()
{
}

void ParticleShader::load()
{
	LOGT("SHADER","ParticleShader::load");

	string vs = readFile_string( shader_path + string("ParticleShader.vs") );
	string ps = readFile_string( shader_path + string("ParticleShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	textureUniform = (UniformSampler2D*) GetUniformByName("tex");
	assert(textureUniform);

	uParticleSize = (UniformFloat*) GetUniformByName("scale");
	assert(uParticleSize);
	uParticleSize->setValue( 2.0f );

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	speedAttribLoc    = glGetAttribLocation( getProgram(), "speed" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

ParticleShader::~ParticleShader(void)
{

}

void ParticleShader::enable( const ShaderParams& params )
{
	glDepthMask(GL_FALSE);

	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(speedAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void ParticleShader::disable()
{
	glDepthMask(GL_TRUE);

	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(speedAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void ParticleShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->speedBuffer );
	glVertexAttribPointer( speedAttribLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );


}

