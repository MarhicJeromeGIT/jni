#include "InstancingShader.h"
#include "fichiers.h"
#include "OpenGLModel.h"

//****************************************************************************
// A simple diffuse color shader
//
//****************************************************************************

InstancingShader::InstancingShader() : Shader()
{
}

void InstancingShader::load()
{
	LOGT("SHADER","InstancingShader::load");

	string vs = readFile_string( shader_path + string("InstancingShader.vs") );
	string ps = readFile_string( shader_path + string("InstancingShader.ps") );
	LoadShaderFromFile(vs,ps);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	uDiffuseColor = (UniformVec4f*) GetUniformByName("color");
	assert(uDiffuseColor != 0);

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
	modelMatrixLoc  = glGetAttribLocation( getProgram(), "ModelMatrix" );
}

InstancingShader::~InstancingShader(void)
{

}

void InstancingShader::enable( const ShaderParams& params )
{
	glUseProgram(_program);
	uViewMat->setValue( params.viewMatrix );
	uProjectionMat->setValue( params.projectionMatrix );

	glEnableVertexAttribArray(vertexAttribLoc);
}


void InstancingShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	Shader::disable();
}

void InstancingShader::Draw( Mesh* m )
{
#ifndef __ANDROID__

	InternalMesh* mesh = (InternalMesh*) m;

	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, mesh->modelMatrixBuffer );
	for (unsigned int i = 0; i < 4 ; i++) 
	{
        glEnableVertexAttribArray(modelMatrixLoc + i);
        glVertexAttribPointer(modelMatrixLoc + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                                (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(modelMatrixLoc + i, 1);
    }


	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );

	if( mesh->nbTriangles < 20000 )
	{
		glDrawElementsInstanced ( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0, mesh->nbInstances );
	}
	else
	{
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_INT, 0 );
	}

#endif

}