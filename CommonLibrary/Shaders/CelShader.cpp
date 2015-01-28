#include "CelShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

using namespace glm;

//****************************************************************************
// Cel shader with texture
//
//****************************************************************************


CelShader::CelShader() : Shader()
{
}

void CelShader::load()
{
	LOGT("SHADER","CelShader::load");

	string vs = readFile_string( shader_path + string("CelShader.vs") );
	string ps = readFile_string( shader_path + string("CelShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uNormalMatrix = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMatrix != 0);

	uDiffuseColor = (UniformVec4f*) GetUniformByName("diffuse_color");
	assert(uDiffuseColor != 0);

	uLightPosition = (UniformVec4f*) GetUniformByName("lightPosition");
	assert(uLightPosition != 0);

	uLightDiffuseColor = (UniformVec4f*) GetUniformByName("lightDiffuseColor");
	assert(uLightDiffuseColor != 0);
	uLightDiffuseColor->setValue( vec4(1.0,1.0,1.0,1.0) );

	uAmbientColor = (UniformVec4f*) GetUniformByName("ambient");
	assert(uAmbientColor != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uUseScreenCoord = (UniformBoolean*) GetUniformByName("bUseScreenCoord");
	assert(uUseScreenCoord != 0);

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
	normalAttribLoc = glGetAttribLocation( getProgram(), "normalOrientation" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

CelShader::~CelShader(void)
{

}

void CelShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	uLightPosition->setValue( params.viewMatrix * params.lights[0].lightPosition );
	uLightDiffuseColor->setValue( params.lights[0].lightDiffuseColor );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMatrix->setValue( normalMatrix );

	uAmbientColor->setValue( vec4(0.4,0.4,0.4,1.0) );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void CelShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void CelShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}

