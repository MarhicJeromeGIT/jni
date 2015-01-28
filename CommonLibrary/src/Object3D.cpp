#include "Object3D.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "ShaderParams.h"

using namespace glm;

Object3D::Object3D()
{
	mesh = NULL;
	material = NULL;
	objectMatrix = mat4(1.0);
}

void Object3D::Draw( MATERIAL_DRAW_PASS Pass )
{
	if( material == NULL || mesh == NULL )
		return;
	
	Shader* shader = material->getShader( Pass );

	ShaderParams::get()->objectMatrix = objectMatrix;
	ShaderParams::get()->modelViewMatrix = ShaderParams::get()->viewMatrix * objectMatrix;

	//the correct matrix to transform the normal is the transpose of the inverse of the (upper top of) M matrix
	glm::mat3 upperTopMV = glm::mat3( ShaderParams::get()->viewMatrix * ShaderParams::get()->objectMatrix);
	glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
	ShaderParams::get()->normalMatrix = normalMatrix;

	shader->enable( *ShaderParams::get() );
	material->SetupUniforms(Pass);
	shader->Draw( mesh );
	shader->disable();
}
