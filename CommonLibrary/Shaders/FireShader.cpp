#include "FireShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

FireShader::FireShader() : Shader()
{
}

void FireShader::load()
{
	string vs = readFile_string( shader_path + string("FireShader.vs") );
	string ps = readFile_string( shader_path + string("FireShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uNormalMat = (UniformMat3f*) GetUniformByName("NormalMatrix");

	uDistortionSampler = (UniformSampler2D*) GetUniformByName("distortion");
	assert(uDistortionSampler != 0);

	uOpacitySampler = (UniformSampler2D*) GetUniformByName("opacity");
	assert(uOpacitySampler != 0);

	uFlameSampler = (UniformSampler2D*) GetUniformByName("flame");
	assert(uFlameSampler != 0);

	uTimeUniform = (UniformFloat*) GetUniformByName("time");
	assert(uTimeUniform != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
	normalAttribLoc  = glGetAttribLocation( getProgram(), "iNormal" );
}

FireShader::~FireShader(void)
{

}

void FireShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	uTimeUniform->setValue( params.time );

	if( uNormalMat != 0 )
	{
		uNormalMat->setValue( params.normalMatrix);
	}

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);
}

void FireShader::disable()
{
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void FireShader::Draw( Mesh* m )
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
