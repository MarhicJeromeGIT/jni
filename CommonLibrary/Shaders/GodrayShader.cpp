#include "GodrayShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"
using namespace glm;

GodrayShader::GodrayShader() : Shader()
{
}

void GodrayShader::load()
{
	LOGT("SHADER","GodrayShader::load\n");

	string vs = readFile_string( shader_path + string("GodrayShader.vs") );
	string ps = readFile_string( shader_path + string("GodrayShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uLightposScreen = (UniformVec2f*) GetUniformByName("LightPosScreen");
	if( uLightposScreen == NULL )
	{
		uLightposScreen = new UniformVec2f( -1, this );
	}

	uWeight = (UniformFloat*) GetUniformByName("weight");
	if( uWeight == NULL )
	{
		uWeight = new UniformFloat( -1, this );
	}

	uDecay = (UniformFloat*) GetUniformByName("decay");
	if( uDecay == NULL )
	{
		uDecay = new UniformFloat( -1, this );
	}

	uExposure = (UniformFloat*) GetUniformByName("exposure");
	if( uExposure == NULL )
	{
		uExposure = new UniformFloat( -1, this );
	}

	vertexAttribLoc    = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc  = glGetAttribLocation( getProgram(), "iTexCoord" );
}

GodrayShader::~GodrayShader(void)
{
}

void GodrayShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void GodrayShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void GodrayShader::Draw( Mesh* m )
{
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