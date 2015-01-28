#include "PhongShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

using namespace glm;

PhongShader::PhongShader() : Shader()
{
}

void PhongShader::load()
{
	LOGT("SHADER","PhongShader::load");

	string vs = readFile_string( shader_path + string("PhongShader.vs") );
	string ps = readFile_string( shader_path + string("PhongShader.ps") );
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

	uAmbientColor = (UniformVec4f*) GetUniformByName("ambient");
	assert(uAmbientColor != 0);

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
	normalAttribLoc = glGetAttribLocation( getProgram(), "normalOrientation" );
}

PhongShader::~PhongShader(void)
{

}

void PhongShader::enable( const ShaderParams& params )
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
}

void PhongShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	Shader::disable();
}

void PhongShader::Draw( Mesh* m )
{
	GLenum errCode;

	while ((errCode = glGetError()) != GL_NO_ERROR) {
		 cout<<"erreur "<<errCode<<endl;
	}

	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );

	while ((errCode = glGetError()) != GL_NO_ERROR) {
		 cout<<"erreur "<<errCode<<endl;
	}
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		while ((errCode = glGetError()) != GL_NO_ERROR) {
		 cout<<"erreur "<<errCode<<endl;
	}
	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	
	while ((errCode = glGetError()) != GL_NO_ERROR) {
		 cout<<"erreur "<<errCode<<endl;
	}
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	while ((errCode = glGetError()) != GL_NO_ERROR) {
		 cout<<"erreur "<<errCode<<endl;
	}

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );

	if( mesh->nbTriangles < 20000 )
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
	else
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
	}

	while ((errCode = glGetError()) != GL_NO_ERROR) {
		 cout<<"erreur "<<errCode<<endl;
	}
}


//****************************************************************************
// Phong shader with texture
//
//****************************************************************************


PhongTextureShader::PhongTextureShader() : Shader()
{
}

void PhongTextureShader::load()
{
	LOGT("SHADER","PhongTextureShader::load");

	string vs = readFile_string( shader_path + string("PhongTextureShader.vs") );
	string ps = readFile_string( shader_path + string("PhongTextureShader.ps") );
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

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
	normalAttribLoc = glGetAttribLocation( getProgram(), "normalOrientation" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

PhongTextureShader::~PhongTextureShader(void)
{

}

void PhongTextureShader::enable( const ShaderParams& params )
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

void PhongTextureShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void PhongTextureShader::Draw( Mesh* m )
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
// Phong shader with depth texture
//
//****************************************************************************

PhongWithDepthShader::PhongWithDepthShader() : Shader()
{
}

void PhongWithDepthShader::load()
{
	LOGT("SHADER","PhongWithDepthShader::load");

	string vs = readFile_string( shader_path + string("PhongDepthShader.vs") );
	string ps = readFile_string( shader_path + string("PhongDepthShader.ps") );
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

	uIntensity = (UniformFloat*) GetUniformByName("intensity");
	assert( uIntensity != 0 );
	
	uSigma = (UniformFloat*) GetUniformByName("sigma");
	assert( uSigma != 0 );

	resX = (UniformFloat*) GetUniformByName("resX");
	if( resX == 0 )
	{
		resX = new UniformFloat( -1, this );
	}

	resY = (UniformFloat*) GetUniformByName("resY");
	if( resY == 0 )
	{
		resY = new UniformFloat( -1, this );
	}

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
	normalAttribLoc = glGetAttribLocation( getProgram(), "normalOrientation" );
//	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

PhongWithDepthShader::~PhongWithDepthShader(void)
{

}

void PhongWithDepthShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	uLightPosition->setValue( params.viewMatrix * params.lights[0].lightPosition );
	uLightDiffuseColor->setValue( params.lights[0].lightDiffuseColor );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMatrix->setValue( normalMatrix );

	uAmbientColor->setValue( vec4(0.4,0.4,0.4,1.0) );

	resX->setValue( params.win_x );
	resY->setValue( params.win_y );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);
//	glEnableVertexAttribArray(texCoordAttribLoc);
}

void PhongWithDepthShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
//	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void PhongWithDepthShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

//	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
//	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

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

