#include "TextureShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"
using namespace glm;
#include "CommonLibrary.h"

//****************************************************************************
// Texture shader and nothing else
//
//****************************************************************************

SimpleTextureShader::SimpleTextureShader() : Shader()
{
	vertexHeader   = "#define SIMPLE_TEXTURE \n";
	fragmentHeader = "#define SIMPLE_TEXTURE \n";
}

void SimpleTextureShader::load()
{
	LOGT("SHADER","SimpleTextureShader::load");

	string vs = vertexHeader   + readFile_string( shader_path + string("SimpleTextureShader.vs") );
	string ps = fragmentHeader + readFile_string( shader_path + string("SimpleTextureShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	color = (UniformVec4f*) GetUniformByName("color");
	assert(color != 0);
	color->setValue(vec4(1,1,1,1));

	// xy scale, zw offset. UV = UV*sx+zw;
	uUVScaleAndOffset = (UniformVec4f*) GetUniformByName("UVScaleAndOffset");
	assert(uUVScaleAndOffset != 0);
	uUVScaleAndOffset->setValue( vec4(1.0,1.0,0.0,0.0) );

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

SimpleTextureShader::~SimpleTextureShader(void)
{
}

void SimpleTextureShader::enable( const ShaderParams& params )
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

void SimpleTextureShader::disable()
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

void SimpleTextureShader::Draw( Mesh* m )
{
#ifndef NDEBUG
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SimpleTextureShader::Draw %d",errCode);
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
	  LOGT("DRAW","Error SimpleTextureShader::Draw glDrawElements %d",errCode);
	  LOGT("DRAW","totalVertices = %d, nbTriangles = %d",mesh->totalVertices, mesh->nbTriangles);
	}
#endif
}

//****************************************************************************
// TextureShader with Per-vertex transparency
//
//****************************************************************************

PerVertexTransparencyTextureShader::PerVertexTransparencyTextureShader() : SimpleTextureShader()
{
	vertexHeader   = "#define VERTEX_TRANSPARENCY \n";
	fragmentHeader = "#define VERTEX_TRANSPARENCY \n";
}

PerVertexTransparencyTextureShader::~PerVertexTransparencyTextureShader(void)
{
}

void PerVertexTransparencyTextureShader::load()
{
	SimpleTextureShader::load();

	vertexTransparencyAttribLoc = glGetAttribLocation( getProgram(), "vertexTransparency" );
}

void PerVertexTransparencyTextureShader::enable( const ShaderParams& params )
{
	SimpleTextureShader::enable(params);
	glEnableVertexAttribArray(vertexTransparencyAttribLoc);
}

void PerVertexTransparencyTextureShader::disable()
{
	glDisableVertexAttribArray(vertexTransparencyAttribLoc);
	SimpleTextureShader::disable();
}

void PerVertexTransparencyTextureShader::Draw( Mesh* m)
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->transparencyBuffer );
	glVertexAttribPointer( vertexTransparencyAttribLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );

	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}

//****************************************************************************
// Texture shader with lighting stuff
//
//****************************************************************************

TextureShader::TextureShader() : Shader()
{
}

void TextureShader::load()
{
	LOGT("SHADER","TextureShader::load");

	string vs = readFile_string( shader_path + string("TextureShader.vs") );
	string ps = readFile_string( shader_path + string("TextureShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uShadowMapSampler = (UniformSampler2D*) GetUniformByName( "shadowmap" );
	assert( uShadowMapSampler != 0 );

	lightMapUniform = (UniformMat4f*) GetUniformByName( "LightMat" );
	assert( lightMapUniform != 0 );

	uClipPlane = (UniformVec4f*) GetUniformByName("clipPlane");
	assert( uClipPlane != 0 );

	uNormalMat = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMat != 0);

	uLightPos = (UniformVec3f*) GetUniformByName("lightPosition");
	assert(uLightPos != 0);

	uAmbientColor = (UniformVec4f*) GetUniformByName("ambientColor");
	assert(uAmbientColor != 0);

	uUseScreenCoord = (UniformBoolean*) GetUniformByName( "bUseScreenCoord" );
	assert( uUseScreenCoord );
	uUseScreenCoord->setValue( false );

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
	normalAttribLoc   = glGetAttribLocation( getProgram(), "normal" );
}

TextureShader::~TextureShader(void)
{

}

void TextureShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	lightMapUniform->setValue( params.lightMap );
	uShadowMapSampler->setValue( params.shadowmap );
	uClipPlane->setValue( params.clipPlane );

	vec4 lightDir =  vec4( vec3(params.lights[0].lightPosition), 0.0 );
	uLightPos->setValue( vec3( params.viewMatrix * lightDir ) );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMat->setValue( normalMatrix );

	uAmbientColor->setValue( params.ambient );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);
}

void TextureShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	glDisableVertexAttribArray(normalAttribLoc);
	Shader::disable();
}

void TextureShader::Draw( Mesh* m )
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
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}

//**********************************************************************
// Multi Texturing
//
//**********************************************************************
#ifndef __ANDROID__

MultiTextureShader::MultiTextureShader(int nbTextures) : Shader()
{
	numTextures = nbTextures;
}

void MultiTextureShader::load()
{

	string vs = readFile_string( shader_path + string("MultiTextureShader.vs") );
	string ps = readFile_string( shader_path + string("MultiTextureShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = new UniformSampler2D*[numTextures];
	uTextureSampler[0] = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler[0] != 0);
	uTextureSampler[1] = (UniformSampler2D*) GetUniformByName("tex2");
	assert(uTextureSampler[1] != 0);

	uShadowMapSampler = (UniformSampler2D*) GetUniformByName( "shadowmap" );
	assert( uShadowMapSampler != 0 );

	lightMapUniform = (UniformMat4f*) GetUniformByName( "LightMat" );
	assert( lightMapUniform != 0 );

	uClipPlane = (UniformVec4f*) GetUniformByName("clipPlane");
	assert( uClipPlane != 0 );

	uNormalMat = (UniformMat3f*) GetUniformByName("NormalMatrix");
	assert(uNormalMat != 0);

	uLightPos = (UniformVec3f*) GetUniformByName("lightPosition");
	assert(uLightPos != 0);

	uAmbientColor = (UniformVec4f*) GetUniformByName("ambientColor");
	assert(uAmbientColor != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );

	texCoordAttribLoc = new int[numTextures];
	texCoordAttribLoc[0] = glGetAttribLocation( getProgram(), "iTexCoord[0]" );
	texCoordAttribLoc[1] = glGetAttribLocation( getProgram(), "iTexCoord[1]" );
	normalAttribLoc = glGetAttribLocation( getProgram(), "normal" );
	textureWeightLoc = glGetAttribLocation( getProgram(), "iTextureWeight" );
}

MultiTextureShader::~MultiTextureShader(void)
{

}

void MultiTextureShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	lightMapUniform->setValue( params.lightMap );
	uShadowMapSampler->setValue( params.shadowmap );
	uClipPlane->setValue( params.clipPlane );

	vec4 lightDir =  vec4( vec3(params.lights[0].lightPosition), 0.0 );

	uLightPos->setValue( vec3( params.viewMatrix * lightDir ) );
	//uLightPos->setValue( vec3( params.lights[0].lightPosition ) );

	glm::mat3 upperTopMV = glm::mat3( params.viewMatrix * params.objectMatrix );
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	uNormalMat->setValue( normalMatrix );

	uAmbientColor->setValue( params.ambient );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc[0]);
	glEnableVertexAttribArray(texCoordAttribLoc[1]);
	glEnableVertexAttribArray(normalAttribLoc);
	glEnableVertexAttribArray(textureWeightLoc);
}

void MultiTextureShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc[0]);
	glDisableVertexAttribArray(texCoordAttribLoc[1]);
	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(textureWeightLoc);

	Shader::disable();
}

void MultiTextureShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
	glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc[0], 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer2 );
	glVertexAttribPointer( texCoordAttribLoc[1], 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->textureWeightBuffer );
	glVertexAttribPointer( textureWeightLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}


//**********************************************************************
// Texture Array Shader
//
//**********************************************************************


TextureArrayShader::TextureArrayShader() : Shader()
{
}

void TextureArrayShader::load()
{
	string vs = readFile_string( shader_path + string("TextureArrayShader.vs") );
	string ps = readFile_string( shader_path + string("TextureArrayShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler2DArray*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	uDiffuseColor = (UniformVec4f*) GetUniformByName("color");
	assert(uDiffuseColor != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

TextureArrayShader::~TextureArrayShader(void)
{
}

void TextureArrayShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	uDiffuseColor->setValue( vec4(1.0,1.0,1.0,1.0) );

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
}

void TextureArrayShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	Shader::disable();
}

void TextureArrayShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordArrayBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}


//****************************************************************************
// Display a slice of a 3D texture
//
//****************************************************************************

TextureSliceShader::TextureSliceShader() : Shader()
{
	vertexHeader   = "#define SIMPLE_TEXTURE \n";
	fragmentHeader = "#define SIMPLE_TEXTURE \n";
}

void TextureSliceShader::load()
{
	LOGT("SHADER","SimpleTextureShader::load");

	string vs = vertexHeader   + readFile_string( shader_path + string("TextureSliceShader.vs") );
	string ps = fragmentHeader + readFile_string( shader_path + string("TextureSliceShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler3D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
}

TextureSliceShader::~TextureSliceShader(void)
{
}

void TextureSliceShader::enable( const ShaderParams& params )
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

void TextureSliceShader::disable()
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

void TextureSliceShader::Draw( Mesh* m )
{
#ifndef NDEBUG
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SimpleTextureShader::Draw %d",errCode);
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
	  LOGT("DRAW","Error SimpleTextureShader::Draw glDrawElements %d",errCode);
	  LOGT("DRAW","totalVertices = %d, nbTriangles = %d",mesh->totalVertices, mesh->nbTriangles);
	}
#endif
}

#endif