#include "OceanShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"
using namespace glm;

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************

OceanShader::OceanShader() : Shader()
{
}

OceanShader::~OceanShader(void)
{
}

void OceanShader::load()
{
	LOGT("SHADER","OceanShader::load");

	string vs = readFile_string( shader_path + string("OceanShader.vs") );
	string ps = readFile_string( shader_path + string("OceanShader.ps") );
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
	if( uTextureSampler == NULL )
	{
		uTextureSampler = new UniformSampler2D( -1, 0, this );
	}

	uCubemapSampler = (UniformSamplerCube*) GetUniformByName( "cubemap" );
	if( uCubemapSampler == NULL )
	{
		uCubemapSampler = new UniformSamplerCube( -1, 0, this );
	}

	uInvViewMat = (UniformMat4f*) GetUniformByName("invviewmat");
	assert(uInvViewMat != 0 );

	uHeightMapSampler = (UniformSampler2D*) GetUniformByName( "heightMapSampler" );
	if( uHeightMapSampler == NULL )
	{
		uHeightMapSampler = new UniformSampler2D( -1, 0, this );
	}

	uNormalMapSampler = (UniformSampler2D*) GetUniformByName( "normalMapSampler" );
	if( uNormalMapSampler == NULL )
	{
		uNormalMapSampler = new UniformSampler2D( -1, 0, this );
	}
	
	lambda = (UniformFloat*) GetUniformByName( "lambda" );
	if( lambda == NULL )
	{
		lambda = new UniformFloat( -1, this );
	}

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
	normalAttribLoc = glGetAttribLocation( getProgram(), "normalOrientation" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );

}

void OceanShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	uLightPosition->setValue( params.viewMatrix * params.lights[0].lightPosition );
	uLightDiffuseColor->setValue( params.lights[0].lightDiffuseColor );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMatrix->setValue( normalMatrix );

	uAmbientColor->setValue( vec4(0.4,0.4,0.4,1.0) );
	uInvViewMat->setValue( glm::inverse(params.viewMatrix) );


	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void OceanShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void OceanShader::Draw( Mesh* m )
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
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
}


//****************************************************************************
// Ocean Shader : Geometry generation
//
//****************************************************************************

OceanGeometryGeneratorShader::OceanGeometryGeneratorShader(void)
{
}

void OceanGeometryGeneratorShader::load()
{
	LOGT("SHADER","OceanGeometryGeneratorShader::load");

	string vs = readFile_string( shader_path + string("OceanGeometryShader.vs") );
	string ps = readFile_string( shader_path + string("OceanGeometryShader.ps") );
	assert( vs.size() > 0 && ps.size() > 0 );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	h0tilde = (UniformSampler2D*) GetUniformByName("h0tilde");
	if( h0tilde == NULL )
	{
		h0tilde = new UniformSampler2D( -1, 0, this );
	}

	h0tildeConj = (UniformSampler2D*) GetUniformByName("h0tildeConj");
	if( h0tildeConj == NULL )
	{
		h0tildeConj = new UniformSampler2D( -1, 0, this );
	}

	N = (UniformFloat*) GetUniformByName("N");
	if( N == NULL )
	{
		N = new UniformFloat( -1, this );
	}

	time = (UniformFloat*) GetUniformByName("time");
	if( time == NULL )
	{
		time = new UniformFloat( -1, this );
	}

	L = (UniformFloat*) GetUniformByName("L");
	if( L == NULL )
	{
		L = new UniformFloat( -1, this );
	}

	multiplicateur = (UniformFloat*) GetUniformByName("multiplicateur");
	if( multiplicateur == NULL )
	{
		multiplicateur = new UniformFloat( -1, this );
	}



	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );

}

OceanGeometryGeneratorShader::~OceanGeometryGeneratorShader(void)
{

}

void OceanGeometryGeneratorShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void OceanGeometryGeneratorShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void OceanGeometryGeneratorShader::Draw( Mesh* m)
{
	GLenum errCode;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
		cout<<errCode<<endl;
	}

	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	while ((errCode = glGetError()) != GL_NO_ERROR) {
		cout<<errCode<<endl;
	}

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	while ((errCode = glGetError()) != GL_NO_ERROR) {
		cout<<errCode<<endl;
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
		cout<<errCode<<endl;
	}

}
