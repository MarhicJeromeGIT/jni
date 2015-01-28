#include "MorphShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************

MorphShader::MorphShader() : Shader()
{
}

MorphShader::~MorphShader(void)
{
}

void MorphShader::load()
{
	LOGT("SHADER","MorphShader::load");

	string vs = readFile_string( shader_path + string("MorphShader.vs") );
	string ps = readFile_string( shader_path + string("MorphShader.ps") );
	assert( vs.size() > 0 && ps.size() > 0 );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uNormalMatrix = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMatrix != 0);

	uDiffuseColor = (UniformVec4f*) GetUniformByName("color");
	assert(uDiffuseColor != 0);

	uMorphCoefficient = (UniformFloat*) GetUniformByName("morphCoeff");
	assert(uMorphCoefficient != 0);

	uLightPosition = (UniformVec4f*) GetUniformByName("lightPosition");
	assert(uLightPosition != 0);

	uLightDiffuseColor = (UniformVec4f*) GetUniformByName("lightDiffuseColor");
	assert(uLightDiffuseColor != 0);

	uAmbientColor = (UniformVec4f*) GetUniformByName("ambient");
	assert(uAmbientColor != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	vertexAttribLoc      = glGetAttribLocation( getProgram(), "vertexPosition" );
	vertexMorphAttribLoc = glGetAttribLocation( getProgram(), "vertexMorphPosition" );
	normalAttribLoc      = glGetAttribLocation( getProgram(), "normalOrientation" );
	normalMorphAttribLoc = glGetAttribLocation( getProgram(), "normalMorphOrientation" );
	texCoordAttribLoc    = glGetAttribLocation( getProgram(), "iTexCoord" );
}

void MorphShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	uLightPosition->setValue( params.viewMatrix * params.lights[0].lightPosition );
	uLightDiffuseColor->setValue( params.lights[0].lightDiffuseColor );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMatrix->setValue( normalMatrix );

	uAmbientColor->setValue( params.ambient );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);
	glEnableVertexAttribArray(vertexMorphAttribLoc);
	glEnableVertexAttribArray(normalMorphAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);

}

void MorphShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(vertexMorphAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(normalMorphAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void MorphShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;

	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBufferMorph );
	glVertexAttribPointer( vertexMorphAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBufferMorph );
	glVertexAttribPointer( normalMorphAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}
