#include "DeferredShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

using namespace glm;

DeferredPassShader::DeferredPassShader() : Shader()
{
}

void DeferredPassShader::load()
{
	LOGT("SHADER","DeferredPassShader::load");

	string vs = readFile_string( shader_path + string("DeferredShader.vs") );
	string ps = readFile_string( shader_path + string("DeferredShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uNormalMatrix = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMatrix != 0);

	uTextureSampler = (UniformSampler2D*)GetUniformByName("tex");
	assert(uTextureSampler != 0 );

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
	normalAttribLoc = glGetAttribLocation( getProgram(), "normalOrientation" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

DeferredPassShader::~DeferredPassShader(void)
{

}

void DeferredPassShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMatrix->setValue( normalMatrix );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void DeferredPassShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void DeferredPassShader::Draw( Mesh* m )
{
	GLenum errCode;

	while ((errCode = glGetError()) != GL_NO_ERROR) {
		 cout<<"erreur "<<errCode<<endl;
	}

	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );

	if( mesh->nbTriangles < 20000 )
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
	else
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
	}
}



//****************************************************************************
// Deferred shading : Full screen pass
//
//****************************************************************************

DeferredShaderFullScreen::DeferredShaderFullScreen() : Shader()
{
}

void DeferredShaderFullScreen::load()
{
	LOGT("SHADER","DeferredShaderFullScreen::load");

	string vs = readFile_string( shader_path + string("DeferredShaderFullScreen.vs") );
	string ps = readFile_string( shader_path + string("DeferredShaderFullScreen.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uNormalMap = (UniformSampler2D*) GetUniformByName("NormalMap");
	assert(uNormalMap != 0);

	uVertexMap = (UniformSampler2D*) GetUniformByName("VertexMap");
	assert(uVertexMap != 0);

	uLightPosition = (UniformVec4f*) GetUniformByName("lightPosition");
	assert(uLightPosition != 0);

	uLightColor = (UniformVec4f*) GetUniformByName("lightColor");
	assert(uLightColor != 0);

	uLightAttenuation = (UniformFloat*) GetUniformByName("lightAttenuation");
	assert(uLightAttenuation != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

DeferredShaderFullScreen::~DeferredShaderFullScreen(void)
{
}

void DeferredShaderFullScreen::enable( const ShaderParams& params )
{
	Shader::enable( params );
	
	uLightAttenuation->setValue( params.lights[0].lightAttenuation );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void DeferredShaderFullScreen::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void DeferredShaderFullScreen::Draw( Mesh* m )
{
	GLenum errCode;

	while ((errCode = glGetError()) != GL_NO_ERROR) {
		 cout<<"erreur "<<errCode<<endl;
	}

	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );

	if( mesh->nbTriangles < 20000 )
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
	else
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
	}
}


//****************************************************************************
// Deferred shading : Full Screen combine pass
//
//****************************************************************************
DeferredShaderLightCombine::DeferredShaderLightCombine() : Shader()
{
}

void DeferredShaderLightCombine::load()
{
	LOGT("SHADER","DeferredShaderLightCombine::load");

	string vs = readFile_string( shader_path + string("LightCombineShader.vs") );
	string ps = readFile_string( shader_path + string("LightCombineShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uColorMap = (UniformSampler2D*) GetUniformByName("colorMap");
	assert(uColorMap != 0);

	uLightMap = (UniformSampler2D*) GetUniformByName("lightMap");
	assert(uLightMap != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

DeferredShaderLightCombine::~DeferredShaderLightCombine(void)
{
}

void DeferredShaderLightCombine::enable( const ShaderParams& params )
{
	Shader::enable( params );
	
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void DeferredShaderLightCombine::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void DeferredShaderLightCombine::Draw( Mesh* m )
{
	GLenum errCode;

	while ((errCode = glGetError()) != GL_NO_ERROR) {
		 cout<<"erreur "<<errCode<<endl;
	}

	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );

	if( mesh->nbTriangles < 20000 )
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
	else
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
	}
}