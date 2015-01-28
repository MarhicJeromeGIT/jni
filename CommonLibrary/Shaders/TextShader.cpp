#include "TextShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"
using namespace glm;

//****************************************************************************
// Texture shader and nothing else
//
//****************************************************************************

ColorTextShader::ColorTextShader() : Shader()
{
}

void ColorTextShader::load()
{
	LOGT("SHADER","ColorTextShader::load");

	string vs = readFile_string( shader_path + string("TextShader.vs") );
	string ps = readFile_string( shader_path + string("TextShader.ps") );
	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uTextureSampler = (UniformSampler2D*) GetUniformByName("tex");
	assert(uTextureSampler != 0);

	// xy scale, zw offset. UV = UV*sx+zw;
	uUVScaleAndOffset = (UniformVec4f*) GetUniformByName("UVScaleAndOffset");
	assert(uUVScaleAndOffset != 0);
	uUVScaleAndOffset->setValue( vec4(1.0,1.0,0.0,0.0) );

	vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
	texCoordAttribLoc = glGetAttribLocation( getProgram(), "iTexCoord" );
	colorAttribLoc    = glGetAttribLocation( getProgram(), "color" );
}

ColorTextShader::~ColorTextShader(void)
{
}

void ColorTextShader::enable( const ShaderParams& params )
{
	Shader::enable( params );
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texCoordAttribLoc);
	glEnableVertexAttribArray(colorAttribLoc);
}

void ColorTextShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	glDisableVertexAttribArray(texCoordAttribLoc);
	glDisableVertexAttribArray(colorAttribLoc);
	Shader::disable();
}

void ColorTextShader::Draw( Mesh* m )
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
	glVertexAttribPointer( texCoordAttribLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->colorBuffer );
	glVertexAttribPointer( colorAttribLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}
