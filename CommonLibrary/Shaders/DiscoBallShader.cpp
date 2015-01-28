#include "DiscoBallShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************

DiscoBallShader::DiscoBallShader() : Shader()
{
}

void DiscoBallShader::load()
{
	string vs = readFile_string( shader_path + string("DiscoBallShader.vs") );
	string ps = readFile_string( shader_path + string("DiscoBallShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uDiffuseColor = (UniformVec4f*) GetUniformByName("color");
	assert(uDiffuseColor != 0);

	uNormalMatrix = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMatrix != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uTime = (UniformFloat*) GetUniformByName("time");
	assert(uTime != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
	normalAttribLoc   = glGetAttribLocation( getProgram(), "normalOrientation" );

}

DiscoBallShader::~DiscoBallShader(void)
{

}

void DiscoBallShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);
	uTime->setValue( params.time );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMatrix->setValue( normalMatrix );

}

void DiscoBallShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	Shader::disable();
}

void DiscoBallShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;

	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
}
