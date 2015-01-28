#include "ShaderList.h"

#include "fichiers.h"
#include "Skybox.h"
#include "OpenGLModel.h"

#ifndef __ANDROID__


StaticMeshShader::StaticMeshShader(void) : Shader()
{
	vertexAttribLoc   = 0;
}

StaticMeshShader::~StaticMeshShader(void)
{
}

void StaticMeshShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
}

void StaticMeshShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	Shader::disable();
}


//*****************************************************
// BlenderShader with ambient, diffuse, specular and a texture
//
//*****************************************************

BlenderShader::BlenderShader() : StaticMeshShader()
{
}

void BlenderShader::load()
{
	string vs = readFile_string( shader_path + string("BlenderShader.vs") );
	string ps = readFile_string( shader_path + string("BlenderShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uLightPos = (UniformVec3f*) GetUniformByName("LightPos");
	assert(uLightPos != 0);

	uClipPlane = (UniformVec4f*) GetUniformByName("clipPlane");
	assert(uClipPlane != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uUseTexture = (UniformBoolean*) GetUniformByName("useTexture");
	assert(uUseTexture != 0);

	uAmbientColor = (UniformVec4f*) GetUniformByName("ambient");
	assert(uAmbientColor);

	uDiffuseColor = (UniformVec4f*) GetUniformByName("diffuse");
	assert(uDiffuseColor);

	uSpecularColor = (UniformVec4f*) GetUniformByName("specular");
	assert(uSpecularColor);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

BlenderShader::~BlenderShader()
{
}

void BlenderShader::Draw( Mesh* m )
{
/*	Clean();

	InternalMesh* mesh = (InternalMesh*) m;

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if( mesh->hasUV )
	{
		glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
		glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0,0 );
	}

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );*/
}

void BlenderShader::enable( const ShaderParams& params )
{
	StaticMeshShader::enable( params );
	glEnableVertexAttribArray(normalAttribLoc);
}

void BlenderShader::disable()
{
	glDisableVertexAttribArray(normalAttribLoc);
	StaticMeshShader::disable();
}

//*****************************************************
// STATIC MESH WITH VBO SHADER
//
//*****************************************************

SimpleStaticMeshShader::SimpleStaticMeshShader() : StaticMeshShader()
{
	string vs = readFile( "BlenderShader.vs" );
	string ps = readFile( "BlenderShader.ps" );
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

SimpleStaticMeshShader::~SimpleStaticMeshShader()
{
}

void SimpleStaticMeshShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer ); 
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0,0 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
}

//*****************************************************
// TEXTURE SHADER + eclairage diffus
//
//*****************************************************

LightStaticMeshShader::LightStaticMeshShader() : StaticMeshShader()
{
	string vs = readFile( "lightstaticMeshShader.vs" );
	string ps = readFile( "lightstaticMeshShader.ps" );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uNormalMat = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMat != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uLightPos = (UniformVec3f*) GetUniformByName("LightPos");
	assert(uLightPos != 0);

	uClipPlane = (UniformVec4f*) GetUniformByName("clipPlane");
	assert(uClipPlane != 0);

//	uClipMat = (UniformMat4f*) GetUniformByName("clippingMat");
//	assert(uClipMat != 0 );

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
	normalAttribLoc   = glGetAttribLocation( getProgram(), "iNormal" );
}

LightStaticMeshShader::~LightStaticMeshShader()
{
}

void LightStaticMeshShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer ); 
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0,0 );

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer ); 
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0,0 );
	

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles*3 , GL_UNSIGNED_INT, 0 );
}

void LightStaticMeshShader::enable( const ShaderParams& params )
{
	StaticMeshShader::enable( params );
	glEnableVertexAttribArray(normalAttribLoc);
}

void LightStaticMeshShader::disable()
{
	glDisableVertexAttribArray(normalAttribLoc);
	StaticMeshShader::disable();
}


//*****************************************************
// SIMPLE SHADOW MAP SHADER
//
//*****************************************************

StaticMeshShadowMapShader::StaticMeshShadowMapShader(void) : StaticMeshShader()
{
	string vs = readFile( "SimpleShadowMap.vs" );
	string ps = readFile( "SimpleShadowMap.ps" );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uShadowMapSampler = (UniformSampler2D*) GetUniformByName("shadowmap");
	assert(uShadowMapSampler != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uLightMat = (UniformMat4f*) GetUniformByName("LightMat");
	assert(uLightMat != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

StaticMeshShadowMapShader::~StaticMeshShadowMapShader(void)
{
}

void StaticMeshShadowMapShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);

	// dessine le mesh :			
	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer ); 
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0,0 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
}


#endif






