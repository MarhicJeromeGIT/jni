#include "WaterShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

using namespace glm;

WaterShader::WaterShader() : Shader()
{
}

void WaterShader::load()
{
	string vs = readFile_string( shader_path + string("waterShader.vs") );
	string ps = readFile_string( shader_path + string("waterShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uNormalMat = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("mirorTex");
	assert(uTextureSampler != 0);

	uWaterTextureSampler = (UniformSampler2D*) GetUniformByName("waterTex");
	assert(uWaterTextureSampler != 0);

	uReflectedProjMat = (UniformMat4f*) GetUniformByName("projReflectionMat");
	assert(uReflectedProjMat != 0);

	uShadowMapSampler = (UniformSampler2D*) GetUniformByName( "shadowmap" );
	assert( uShadowMapSampler != 0 );

	lightMapUniform = (UniformMat4f*) GetUniformByName( "LightMat" );
	assert( lightMapUniform != 0 );

	uNoiseSampler = (UniformSampler2D*) GetUniformByName( "noiseMap" );
	//assert( uNoiseSampler != 0 );

	uLightPos = (UniformVec3f*) GetUniformByName("LightPos");
	assert( uLightPos != 0 );

	uTime = (UniformFloat*)  GetUniformByName("time");
	assert( uTime != 0 );

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
	normalAttribLoc     = glGetAttribLocation( getProgram(), "iNormale" );
	tangenteAttribLoc   = glGetAttribLocation( getProgram(), "iTangente" );
}

WaterShader::~WaterShader(void)
{

}

void WaterShader::enable( const ShaderParams& params )
{
	Shader::enable( params );

	vec4 lightDir = vec4( params.lights[0].lightPosition );
	//vec4 lightDir =  vec4( vec3(params.lights[0].lightPosition), 0.0 );
	uLightPos->setValue( vec3( params.viewMatrix * lightDir ) );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMat->setValue( normalMatrix );

	uTextureSampler->setValue( params.watermap );
	uTime->setValue( params.time );

	lightMapUniform->setValue( params.lightMap );
	uShadowMapSampler->setValue( params.shadowmap );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);
	glEnableVertexAttribArray(tangenteAttribLoc);
}

void WaterShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(tangenteAttribLoc);
	Shader::disable();
}

void WaterShader::Draw( Mesh* m )
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
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
}
