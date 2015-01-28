#include "FullScreenEffectShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

//****************************************************************************
// A Sobel Shader
//
//****************************************************************************

SobelShader::SobelShader() : Shader()
{
}

void SobelShader::load()
{
	LOGT("SHADER","SobelShader::load");

	string vs = readFile_string( shader_path + string("SobelShader.vs") );
	string ps = readFile_string( shader_path + string("SobelShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	textureUniform = (UniformSampler2D*) GetUniformByName("tex");
	assert(textureUniform);

	uTexWidth = (UniformFloat*) GetUniformByName("texWidth");
	assert(uTexWidth);

	uTexHeight = (UniformFloat*) GetUniformByName("texHeight");
	assert(uTexHeight);

	UniformFloat* uTexWidth;
	UniformFloat* uTexHeight;

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "iVertex" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

SobelShader::~SobelShader(void)
{

}

void SobelShader::enable( const ShaderParams& params )
{
	//glDepthMask(GL_FALSE);

	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void SobelShader::disable()
{
	//glDepthMask(GL_TRUE);

	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void SobelShader::Draw( Mesh* m )
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
// A Line Shader (Gaussian Blur ?)
//
//****************************************************************************


LineShader::LineShader() : Shader()
{
}

void LineShader::load()
{
	string vs = readFile_string( shader_path + string("LineShader.vs") );
	string ps = readFile_string( shader_path + string("LineShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

LineShader::~LineShader(void)
{
}

void LineShader::enable( const ShaderParams& params )
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
	glEnableVertexAttribArray(texCoordAttribLoc);

#ifndef NDEBUG
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SimpleTextureShader::enable %d",errCode);
	}
#endif
}

void LineShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();

#ifndef NDEBUG
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SimpleTextureShader::disable %d",errCode);
	}
#endif
}

void LineShader::Draw( Mesh* m )
{
#ifndef NDEBUG
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error LineShader::Draw %d",errCode);
	}
#endif

	InternalMesh* mesh = (InternalMesh*) m;
	Clean();



	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );

#ifndef NDEBUG
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SimpleTextureShader::CLEAN %d",errCode);
	}
#endif

	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );

#ifndef NDEBUG
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error LineShader::Draw glDrawElements %d",errCode);
	  LOGT("DRAW","totalVertices = %d, nbTriangles = %d",mesh->totalVertices, mesh->nbTriangles);
	}
#endif
}


//****************************************************************************
// A texture mix shader
//
//****************************************************************************


TextureMixShader::TextureMixShader() : Shader()
{
}

void TextureMixShader::load()
{
	LOGT("SHADER","TextureMixShader::load");

	string vs = readFile_string( shader_path + string("TextureMixShader.vs") );
	string ps = readFile_string( shader_path + string("TextureMixShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler1 = (UniformSampler2D*) GetUniformByName("tex1");
	assert(uTextureSampler1 != 0);

	uTextureSampler2 = (UniformSampler2D*) GetUniformByName("tex2");
	assert(uTextureSampler2 != 0);

	uTextureMix = (UniformBoolean*)  GetUniformByName("textureMix");
	assert(uTextureMix != 0);

	uTextureMul = (UniformBoolean*)  GetUniformByName("textureMul");
	assert(uTextureMul != 0);

	uTextureDiff = (UniformBoolean*)  GetUniformByName("textureDiff");
	if( uTextureDiff == NULL )
	{
		uTextureDiff = new UniformBoolean( -1 , this );
	}

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

TextureMixShader::~TextureMixShader(void)
{
}

void TextureMixShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void TextureMixShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void TextureMixShader::Draw( Mesh* m )
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
// Gaussian Blur (Vertical and Horizontal pass)
//
//****************************************************************************


GaussianBlurShader::GaussianBlurShader() : Shader()
{
}

void GaussianBlurShader::load()
{
	string vs = readFile_string( shader_path + string("GaussianBlurShader.vs") );
	string ps = readFile_string( shader_path + string("GaussianBlurShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uVerticalPass = (UniformBoolean*) GetUniformByName("isVerticalPass");
	assert(uVerticalPass != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

GaussianBlurShader::~GaussianBlurShader(void)
{
}

void GaussianBlurShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void GaussianBlurShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void GaussianBlurShader::Draw( Mesh* m )
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


