#include "ShockwaveShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"
using namespace glm;
#include "CommonLibrary.h"

//****************************************************************************
// Shockwave Shader Private
//
//****************************************************************************

ShockwaveShaderPrivate::ShockwaveShaderPrivate()
{
}

void ShockwaveShaderPrivate::load()
{
	LOGT("SHADER","ShockwaveShaderPrivate::load");

	string vs = vertexHeader   + readFile_string( shader_path + string("ShockwaveShader.vs") );
	string ps = fragmentHeader + readFile_string( shader_path + string("ShockwaveShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uNormalMat = (UniformMat3f*) GetUniformByName("NormalMatrix");
	if( uNormalMat == NULL )
	{
		uNormalMat = new UniformMat3f(0,this);
	}

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
}

ShockwaveShaderPrivate::~ShockwaveShaderPrivate(void)
{
}

void ShockwaveShaderPrivate::enable( const ShaderParams& params )
{
#ifndef NDEBUG
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SimpleTextureShader::enable begin %d",errCode);
	}
#endif

	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);

//	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
//	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
//	uNormalMat->setValue( normalMatrix );

#ifndef NDEBUG
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error ShockwaveShader::enable %d",errCode);
	}
#endif
}

void ShockwaveShaderPrivate::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	Shader::disable();

#ifndef NDEBUG
	GLenum errCode;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error ShockwaveShader::disable %d",errCode);
	}
#endif
}

void ShockwaveShaderPrivate::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );

	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );

}

//****************************************************************************
// Shockwave Shader
//
//****************************************************************************

ShockwaveShader::ShockwaveShader() : ShockwaveShaderPrivate()
{
}

ShockwaveShader::~ShockwaveShader(void)
{
}

void ShockwaveShader::load()
{
	LOGT("SHADER","ShockwaveShader::load");
	ShockwaveShaderPrivate::load();

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uTime = (UniformFloat*) GetUniformByName("uTime");
	if( uTime == NULL )
	{
		uTime = new UniformFloat(0,this);
	}

}



