#include "ShadowMapGenerator.h"
#include "fichiers.h"
#include "OpenGLModel.h"

//*****************************************************
// GENERE UNE SHADOW MAP
//
//*****************************************************

ShadowMapGenerator::ShadowMapGenerator(void) : Shader()
{
}

void ShadowMapGenerator::load()
{
	LOGT("SHADER","ShadowMapGenerator::load");

	string vs = readFile_string( shader_path + string("shadowMapGenerator.vs") );
	string ps = readFile_string( shader_path + string("shadowMapGenerator.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	objectCol = (UniformFloat*) GetUniformByName("objectCol");
	if( objectCol == NULL )
	{
		objectCol = new UniformFloat( -1, this );
	}
	objectCol->setValue( 0.0f );

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
}

ShadowMapGenerator::~ShadowMapGenerator(void)
{
}

void ShadowMapGenerator::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
}

void ShadowMapGenerator::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	Shader::disable();
}

void ShadowMapGenerator::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	// dessine le mesh :
	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

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

