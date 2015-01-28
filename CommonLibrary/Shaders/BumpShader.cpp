#include "BumpShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"
using namespace glm;

BumpShader::BumpShader() : Shader()
{
	fragmentHeader = "";
}

void BumpShader::load()
{
	LOGT("SHADER","BumpShader::load\n");

	string vs = readFile_string( shader_path + string("BumpShader.vs") );
	string ps = fragmentHeader + readFile_string( shader_path + string("BumpShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("colorMap");
	assert(uTextureSampler != 0);

	uNormalMap = (UniformSampler2D*) GetUniformByName( "normalMap" );
	assert( uNormalMap != 0 );

	uNormalMat = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMat != 0);

	uOutline = (UniformBoolean*) GetUniformByName("outline");
	assert(uOutline != 0);

	lightMatUniform = (UniformMat4f*) GetUniformByName( "LightMat" );
	
	uShadowMapSampler = (UniformSampler2D*) GetUniformByName( "shadowmap" );
		
	// Lighting :
	uLightPos = (UniformVec3f*) GetUniformByName("lightPos");
	assert(uLightPos != 0);

	uLightAmbient = (UniformVec3f*) GetUniformByName("light_ambient");
	if( uLightAmbient == NULL )
	{
		uLightAmbient = new UniformVec3f( -1, this );
	}

	uLightDiffuse = (UniformVec3f*) GetUniformByName("light_diffuse");
	if( uLightDiffuse == NULL )
	{
		uLightDiffuse = new UniformVec3f( -1, this );
	}

	uLightSpecular = (UniformVec3f*) GetUniformByName("light_specular");
	if( uLightSpecular == NULL )
	{
		uLightSpecular = new UniformVec3f( -1, this );
	}

	uShininess = (UniformFloat*) GetUniformByName("shininess");
	if( uShininess == NULL )
	{
		uShininess = new UniformFloat( -1, this );
	}

	vertexAttribLoc    = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc  = glGetAttribLocation( getProgram(), "iTexcoord" );
	normalAttribLoc     = glGetAttribLocation( getProgram(), "normale" );
	tangenteAttribLoc   = glGetAttribLocation( getProgram(), "tangente" );
}

BumpShader::~BumpShader(void)
{
}

void BumpShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	uLightAmbient->setValue( vec3( params.ambient ) );
	uLightDiffuse->setValue( vec3( params.lights[0].lightDiffuseColor ) );
	uLightDiffuse->setValue( params.lights[0].lightSpecularColor );
	uShininess->setValue( params.lights[0].lightShininess );

	if( lightMatUniform != NULL )
	{
		lightMatUniform->setValue( params.lightMap );
	}
	
	if( uShadowMapSampler )
	{
		uShadowMapSampler->setValue( params.shadowmap );
	}

	vec4 lightDir =  vec4( vec3(params.lights[0].lightPosition), 0.0 );
	uLightPos->setValue( vec3( params.viewMatrix * lightDir ) );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMat->setValue( normalMatrix );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);
	glEnableVertexAttribArray(tangenteAttribLoc);
}

void BumpShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(tangenteAttribLoc);
	Shader::disable();
}

void BumpShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->tangenteBuffer );
	glVertexAttribPointer( tangenteAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

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
// Bump mapping shader with specular map
//
//****************************************************************************


BumpSpecularMapShader::BumpSpecularMapShader() : BumpShader()
{
	fragmentHeader = "#define SPECULAR_MAP \n #define DIFFUSE_MULT \n";
}

BumpSpecularMapShader::~BumpSpecularMapShader()
{
}

void BumpSpecularMapShader::load()
{
	LOGT("SHADER","BumpSpecularMapShader::load\n");

	BumpShader::load();

	uSpecularMap = (UniformSampler2D*) GetUniformByName("specularMap" );
	assert( uSpecularMap != 0 );

	uDiffuseMult = (UniformVec4f*) GetUniformByName("diffuseMult" );
	assert( uDiffuseMult != 0 );
}

//****************************************************************************
// Bump mapping shader with specular map mixing diffuse values
//
//****************************************************************************

BumpMixSpecularMapShader::BumpMixSpecularMapShader(void) : BumpSpecularMapShader()
{
	fragmentHeader = "#define SPECULAR_MAP \n #define DIFFUSE_MIX \n";
}

BumpMixSpecularMapShader::~BumpMixSpecularMapShader(void)
{
}

void BumpMixSpecularMapShader::load()
{
	LOGT("SHADER","BumpMixSpecularMapShader::load\n");

	BumpSpecularMapShader::load();

	uDiffuseMult2 = (UniformVec4f*) GetUniformByName("diffuseMult2" );
	assert( uDiffuseMult2 != 0 );
}

