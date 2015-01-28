#include "SkyboxShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"


//*****************************************************
// SKYBOX SHADER
//
//*****************************************************

SkyboxShader::SkyboxShader(void) : Shader()
{
}

void SkyboxShader::load()
{
	LOGT("SHADER","SkyboxShader::load");

	string vs = readFile_string( shader_path + string("skyboxShader.vs") );
	string ps = readFile_string( shader_path + string("skyboxShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uCubeMapSampler = (UniformSamplerCube*) GetUniformByName("gCubemapTexture");
	assert(uCubeMapSampler != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
}

void SkyboxShader::enable( const ShaderParams& params )
{
	Shader::enable(params);

//	glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
//	glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	//glDisable(GL_CULL_FACE);

	glEnableVertexAttribArray(vertexAttribLoc);

#ifndef NDEBUG
	GLenum errCode;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SkyboxShader::enable %d",errCode);
	}
#endif
}

void SkyboxShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	Shader::disable();

	glCullFace(GL_BACK);
//	glCullFace(OldCullFaceMode);
//	glDepthFunc(OldDepthFuncMode);

#ifndef NDEBUG
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SkyboxShader::disable %d",errCode);
	}
#endif
}


SkyboxShader::~SkyboxShader(void)
{
}

void SkyboxShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

#ifndef NDEBUG
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SkyboxShader::Draw Clean %d",errCode);
	}
#endif

	// dessine le mesh :
	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );

#ifndef NDEBUG
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SkyboxShader::Draw glDrawElements %d",errCode);
	}
#endif
}

