#include "TesselationShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"
#include "AntigravityConfig.h"

//****************************************************************************
// Terrain tesselation shader
//
//****************************************************************************

TerrainTesselationShader::TerrainTesselationShader() : Shader()
{
	load();
}

void TerrainTesselationShader::load()
{
	LOGT("SHADER","TerrainTesselationShader::load");

	string vs = readFile( SHADER_PATH "TerrainTesselationShader.vs" );
	string ps = readFile( SHADER_PATH "TerrainTesselationShader.ps" );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uDiffuseColor = (UniformVec4f*) GetUniformByName("color");
	assert(uDiffuseColor != 0);

	uHeightmap = (UniformSampler2D*) GetUniformByName( "heightmap" );
	assert(uHeightmap != 0);

	uIntensity = (UniformFloat*) GetUniformByName( "intensity" );
	assert(uIntensity != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "texCoord" );
}

TerrainTesselationShader::~TerrainTesselationShader(void)
{

}

void TerrainTesselationShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void TerrainTesselationShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void TerrainTesselationShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;

	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
}
