
#include "OpenGLModel.h"
#include "Model.h"

#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include "TextureGL.h"
#include "ShaderList.h"

#include "Material.h"

#include "SkinnedShader.h"
#include "DiffuseShader.h"
#include <algorithm>
using namespace std;
#include "CommonLibrary.h"
#include "MeshAnimation.h"
#include "Animation.h"


bool sortMesh( const Mesh& m1, const Mesh& m2 )
{
	return m1.name < m2.name;
}

//**************************************************
// Internal Mesh
//
//**************************************************

InternalMesh::InternalMesh()
{
	totalVertices = 0;
	nbTriangles = 0;
	indicesStart = 0;

	hasAnimations = false;
	hasBones = false;
	hasNormals = false;
	hasUVs = false;

	vertexBuffer = 0;
	bonesIndexBuffer = 0;
	bonesWeightBuffer = 0;
	normalBuffer = 0;
	tangenteBuffer = 0;
	bitangenteBuffer = 0;

	texCoordBuffer = 0;

	textureWeightBuffer = 0;
	hasSecondUVSet = false;
	texCoordBuffer2 = 0;

	texCoordArrayBuffer = 0;
	indicesBuffer = 0;
	speedBuffer = 0;

	hasDiffuseMap = false;
	texture = NULL;
	hasNormalMap = false;
	normalmap = NULL;
	hasSpecularMap = false;
	specularMap = NULL;

	isTranparent = false;

	vertexBufferMorph = 0;
	normalBufferMorph = 0;

	colorBuffer        = 0;
	transparencyBuffer = 0;
}

InternalMesh::~InternalMesh()
{
	//releaseGLData();
}

void InternalMesh::releaseGLData() // called manually.
{
	return; // Don't do that

	if( glIsBuffer(vertexBuffer) )
	{
		glDeleteBuffers(1,&vertexBuffer);
	}
	if( glIsBuffer(bonesIndexBuffer) )
	{
		glDeleteBuffers(1,&bonesIndexBuffer);
	}
	if( glIsBuffer(bonesWeightBuffer) )
	{
		glDeleteBuffers(1,&bonesWeightBuffer);
	}
	if( glIsBuffer(normalBuffer) )
	{
		glDeleteBuffers(1,&normalBuffer);
	}
	if( glIsBuffer(tangenteBuffer) )
	{
		glDeleteBuffers(1,&tangenteBuffer);
	}
	if( glIsBuffer(bitangenteBuffer) )
	{
		glDeleteBuffers(1,&bitangenteBuffer);
	}
	if( glIsBuffer(texCoordBuffer) )
	{
		glDeleteBuffers(1,&texCoordBuffer);
	}
	if( glIsBuffer(textureWeightBuffer) )
	{
		glDeleteBuffers(1,&textureWeightBuffer);
	}
	if( glIsBuffer(texCoordBuffer2) )
	{
		glDeleteBuffers(1,&texCoordBuffer2);
	}
	if( glIsBuffer(texCoordArrayBuffer) )
	{
		glDeleteBuffers(1,&texCoordArrayBuffer);
	}
	if( glIsBuffer(indicesBuffer) )
	{
		glDeleteBuffers(1,&indicesBuffer);
	}
	if( glIsBuffer(speedBuffer) )
	{
		glDeleteBuffers(1,&speedBuffer);
	}
	if( glIsBuffer(vertexBufferMorph) )
	{
		glDeleteBuffers(1,&vertexBufferMorph);
	}
	if( glIsBuffer(normalBufferMorph) )
	{
		glDeleteBuffers(1,&normalBufferMorph);
	}
}


//**************************************************
// Dynamic Mesh
//
//**************************************************

DynamicMesh::DynamicMesh(int mV, int mT) : Mesh(), maxVertices(mV), maxTriangles(mT)
{
	// init VBOs with the max number of vertices.
	mesh.name = "test";
	mesh.totalVertices = maxVertices;
	mesh.nbTriangles   = maxTriangles;

	glGenBuffers(1,&mesh.vertexBuffer);
	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * maxVertices, NULL, GL_DYNAMIC_DRAW );

	glGenBuffers(1,&mesh.texCoordBuffer);
	glBindBuffer( GL_ARRAY_BUFFER, mesh.texCoordBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec2) * maxVertices, NULL, GL_DYNAMIC_DRAW );

	glGenBuffers(1,&mesh.indicesBuffer);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.indicesBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 3 * maxTriangles, NULL, GL_DYNAMIC_DRAW );
}

DynamicMesh::~DynamicMesh()
{
	mesh.releaseGLData();
	maxTriangles = 0;
	maxVertices = 0;
}


void DynamicMesh::updateGeometry( vec3* vertices, vec2* texCoord, int nbVertices, unsigned short* triangles, int nbTriangles )
{
	mesh.totalVertices = nbVertices;
	mesh.nbTriangles   = nbTriangles;

	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * nbVertices, vertices, GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, mesh.texCoordBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec2) * nbVertices, texCoord, GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.indicesBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 3 * nbTriangles, triangles, GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void DynamicMesh::updateGeometry( vec3* vertices, vec2* texCoord, int nbVertices, unsigned int* triangles, int nbTriangles )
{
	mesh.totalVertices = nbVertices;
	mesh.nbTriangles   = nbTriangles;

	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * nbVertices, vertices, GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, mesh.texCoordBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec2) * nbVertices, texCoord, GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.indicesBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * nbTriangles, triangles, GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void DynamicMesh::updateGeometry( vec3* vertices, vec3* texCoord, int nbVertices, unsigned short* triangles, int nbTriangles )
{
	mesh.totalVertices = nbVertices;
	mesh.nbTriangles   = nbTriangles;

	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * nbVertices, vertices, GL_DYNAMIC_DRAW );

	if( mesh.texCoordArrayBuffer == 0 )
	{
		glGenBuffers(1,&mesh.texCoordArrayBuffer);
		glBindBuffer( GL_ARRAY_BUFFER, mesh.texCoordArrayBuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * mesh.totalVertices, NULL, GL_DYNAMIC_DRAW );
	}
	glBindBuffer( GL_ARRAY_BUFFER, mesh.texCoordArrayBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * nbVertices, texCoord, GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.indicesBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 3 * nbTriangles, triangles, GL_DYNAMIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

OpenGLModel::OpenGLModel( const Model* model) : Mesh(), internModel(model)
{
	reloadGLData();
}

static TextureGL* loadTexture( const string& basename )
{
	cout<<"attempting to load "<<basename<<endl;
	string name =  basename;
	TextureGL* thisTexture = TextureManager::get()->loadTexture(  basename, name);

	assert( thisTexture != NULL );
	assert( thisTexture->error == false );

	if( thisTexture == NULL || thisTexture->error != false )
	{
		LOGE("error with texture %s",basename.c_str());
	}


	return thisTexture;
}

bool OpenGLModel::loadMesh( const Model::Mesh* mesh )
{
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGE("ERROR before loadMesh %d",errCode);
	}

	InternalMesh newMesh;

	newMesh.name = mesh->name;
	newMesh.totalVertices = mesh->nbVertices;
	newMesh.nbTriangles   = mesh->nbTriangles;

	bool hasPositionAnimation = mesh->numMeshAnim > 0 && mesh->shapekeys[0].hasPositions;
	glGenBuffers(1,&newMesh.vertexBuffer);
	glBindBuffer( GL_ARRAY_BUFFER, newMesh.vertexBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * newMesh.totalVertices, mesh->vertex_positions, hasPositionAnimation ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );


	glGenBuffers(1,&newMesh.indicesBuffer);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, newMesh.indicesBuffer );
	if( newMesh.nbTriangles < 20000 )
	{
		unsigned short* arrayIndices = new unsigned short[newMesh.nbTriangles*3];
		for( int i=0; i< newMesh.nbTriangles; i++ )
		{
			ivec3 tri = mesh->indices[i];
			assert(tri.x < mesh->nbVertices && tri.y < mesh->nbVertices && tri.z < mesh->nbVertices );

			arrayIndices[i*3]   = tri.x;
			arrayIndices[i*3+1] = tri.y;
			arrayIndices[i*3+2] = tri.z;
		}
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 3 * newMesh.nbTriangles, arrayIndices, GL_STATIC_DRAW );
		delete[] arrayIndices;
	}
	else
	{
		unsigned int* arrayIndices = new unsigned int[newMesh.nbTriangles*3];
		for( int i=0; i< newMesh.nbTriangles; i++ )
		{
			ivec3 tri = mesh->indices[i];
			assert(tri.x < mesh->nbVertices && tri.y < mesh->nbVertices && tri.z < mesh->nbVertices );

			arrayIndices[i*3]   = tri.x;
			arrayIndices[i*3+1] = tri.y;
			arrayIndices[i*3+2] = tri.z;
		}
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * newMesh.nbTriangles, arrayIndices, GL_STATIC_DRAW );
		delete[] arrayIndices;
	}
	

	newMesh.hasBones = mesh->hasBones;
	if( newMesh.hasBones )
	{
		vec4* newArray = new vec4[newMesh.totalVertices];
		for( int i=0; i< newMesh.totalVertices; i++ )
		{
			newArray[i] = vec4( (float)mesh->bonesIndex[i].x,(float)mesh->bonesIndex[i].y,(float)mesh->bonesIndex[i].z,(float)mesh->bonesIndex[i].w);
		}

		glGenBuffers(1,&newMesh.bonesIndexBuffer);
		glBindBuffer( GL_ARRAY_BUFFER, newMesh.bonesIndexBuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vec4) * newMesh.totalVertices, newArray, GL_STATIC_DRAW );

		delete[] newArray;

		glGenBuffers(1,&newMesh.bonesWeightBuffer);
		glBindBuffer( GL_ARRAY_BUFFER, newMesh.bonesWeightBuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vec4) * newMesh.totalVertices, mesh->bonesWeight, GL_STATIC_DRAW );
	}

	glGenBuffers(1,&newMesh.texCoordBuffer);
	glBindBuffer( GL_ARRAY_BUFFER, newMesh.texCoordBuffer);
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec2) * newMesh.totalVertices, mesh->vertex_uvs, GL_STATIC_DRAW );

	glGenBuffers(1,&newMesh.normalBuffer);
	glBindBuffer( GL_ARRAY_BUFFER, newMesh.normalBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * newMesh.totalVertices, mesh->vertex_normales, GL_STATIC_DRAW );

	glGenBuffers(1,&newMesh.tangenteBuffer);
	glBindBuffer( GL_ARRAY_BUFFER, newMesh.tangenteBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * newMesh.totalVertices, mesh->vertex_tangentes, GL_STATIC_DRAW );

	glGenBuffers(1,&newMesh.bitangenteBuffer);
	glBindBuffer( GL_ARRAY_BUFFER, newMesh.bitangenteBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * newMesh.totalVertices, mesh->vertex_bitangentes, GL_STATIC_DRAW );

	newMesh.hasSecondUVSet = mesh->hasSecondUVSet;
	if( newMesh.hasSecondUVSet )
	{
		glGenBuffers(1,&newMesh.texCoordBuffer2);
		glBindBuffer( GL_ARRAY_BUFFER, newMesh.texCoordBuffer2);
		glBufferData( GL_ARRAY_BUFFER, sizeof(vec2) * newMesh.totalVertices, mesh->vertex_uvs2, GL_STATIC_DRAW );
	}

	if ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGE("ERROR during loadmesh %d",errCode);
	}

	// load the texture :
	newMesh.isTranparent = false;
	unsigned int materialIndex = mesh->materialIndex;
	newMesh.hasDiffuseMap  = false;
	newMesh.hasNormalMap   = false;
	newMesh.hasSpecularMap = false;
	if( materialIndex >= 0 )
	{
		newMesh.hasDiffuseMap = internModel->materials[materialIndex].hasDiffuseTexture;
		if(newMesh.hasDiffuseMap )
		{
			LOGE("Loading model default texture : %s", internModel->materials[materialIndex].diffuse_texture.c_str() );
			newMesh.texture = loadTexture( internModel->materials[materialIndex].diffuse_texture );
			newMesh.isTranparent = newMesh.texture->getTransparency();
		}

		newMesh.hasNormalMap = internModel->materials[materialIndex].hasNormalTexture;
		if( newMesh.hasNormalMap )
		{
			newMesh.normalmap = loadTexture( internModel->materials[materialIndex].normal_texture );
		}

		newMesh.hasSpecularMap = internModel->materials[materialIndex].hasSpecularTexture;
		if( newMesh.hasSpecularMap )
		{
			newMesh.specularMap = loadTexture( internModel->materials[materialIndex].specular_texture );
		}
	}

	// load the shapekeys
	newMesh.numShapeKeys = mesh->numMeshAnim;
	newMesh.shapekeys = new ShapeKeyBuffers[newMesh.numShapeKeys];
	for( int anim = 0; anim < mesh->numMeshAnim; anim++ )
	{
		newMesh.shapekeys[anim].hasPositions = mesh->shapekeys[anim].hasPositions;
		if( newMesh.shapekeys[anim].hasPositions )
		{
			glGenBuffers(1,&newMesh.shapekeys[anim].vertexBuffer);
			glBindBuffer( GL_ARRAY_BUFFER, newMesh.shapekeys[anim].vertexBuffer);
			glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * newMesh.totalVertices, mesh->shapekeys[anim].vertex_meshanim_positions, GL_STATIC_DRAW );
		}

		newMesh.shapekeys[anim].hasNormal = mesh->shapekeys[anim].hasNormales;
		if( newMesh.shapekeys[anim].hasNormal )
		{
			glGenBuffers(1,&newMesh.shapekeys[anim].normalBuffer);
			glBindBuffer( GL_ARRAY_BUFFER, newMesh.shapekeys[anim].normalBuffer);
			glBufferData( GL_ARRAY_BUFFER, sizeof(vec3) * newMesh.totalVertices, mesh->shapekeys[anim].vertex_meshanim_normales, GL_STATIC_DRAW );
		}
	}


	if ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGE("ERROR after loadMesh %d",errCode);
	}

	meshes.push_back( newMesh );
	cout<<"submesh loaded :"<<mesh->name<<endl;

	return true;
}

OpenGLModel::~OpenGLModel(void)
{
}

void OpenGLModel::update( float dt )
{
}

void OpenGLModel::reloadGLData()
{
	assert( internModel != NULL );

	meshes.clear();

	// creates all the VBOs
	// un mesh est compose de plusieurs sous-meshes
	for( unsigned int i=0; i< internModel->meshes.size(); i++ )
	{
		loadMesh( &(internModel->meshes[i]) );
	}
}

//*********************************
// Static model
//
//*********************************

OpenGLStaticModel::OpenGLStaticModel( const Model* internModel ) : OpenGLModel( internModel )
{
}

//*********************************
// skinned model
//
//*********************************

OpenGLSkinnedModel::OpenGLSkinnedModel( const Model* internModel ) : OpenGLModel( internModel )
{
}

//*********************************
// Parent class for model instances
//
//*********************************

OpenGLModelInstance::OpenGLModelInstance( const mat4& mat, OpenGLModel* glModel )
{
	objectMatrix = mat;

	// compute the bounding box :
	boundingBox = new AABB(objectMatrix, glModel->internModel);

	position = vec3( objectMatrix[3][0], objectMatrix[3][1], objectMatrix[3][2]);

	for( unsigned int i=0; i< glModel->internModel->meshes.size(); i++ )
	{
		meshesToDraw.push_back(i);
	}
}

OpenGLModelInstance::~OpenGLModelInstance()
{
	delete boundingBox;
}

void OpenGLModelInstance::SetPosition( const vec3& pos )
{
	position = pos;
	objectMatrix[3][0] = position.x;
	objectMatrix[3][1] = position.y;
	objectMatrix[3][2] = position.z;

	// recompute the bounding box
	delete boundingBox;
	boundingBox = new AABB(objectMatrix, internModel);
}

bool OpenGLModelInstance::intersectRay( vec3& pos, const vec3& rayStart, const vec3& rayDir )
{
	bool intersect = boundingBox->rayIntersection( mat4(1.0), rayStart, rayStart + rayDir * 400.0f );
	return intersect;
}

int OpenGLModelInstance::getMeshIndex( const char* meshName )
{
	int index = -1;
	for( int i=0; i< glModel->internModel->meshes.size(); i++ )
	{
		if( strcmp( meshName, internModel->meshes[i].name.c_str() ) == 0 )
		{
			index = i;
			break;
		}
	}
	return index;
}

//*********************************
// One instance of a static mesh
//
//*********************************

OpenGLStaticModelInstance::OpenGLStaticModelInstance( const mat4& mat, OpenGLModel * _glModel, const Model* _internModel /*=NULL*/ ) : OpenGLModelInstance(mat, _glModel )
{
	internModel = _glModel->internModel;
	//glModel = dynamic_cast<OpenGLStaticModel*>(_glModel);
	glModel = (OpenGLStaticModel*)_glModel;
	assert(glModel != NULL );

	animation = NULL;
	meshAnimation = NULL;
	if( internModel->animations.size() > 0 && internModel->animations[0].channels.size() > 0 )
	{
		animation = new Animation( internModel, 0 );
		meshAnimation = new MeshAnimation( internModel );
	}

	for( int meshIndex = 0; meshIndex < internModel->meshes.size(); meshIndex++ )
	{
		sparePartMatrix.push_back( mat4(1.0) );

		int matIndex = internModel->meshes[meshIndex].materialIndex;

		cout<<"material name = "<<internModel->materials[ internModel->meshes[meshIndex].materialIndex ].name<<endl;

		if( glModel->meshes[meshIndex].hasDiffuseMap )
		{
			if( glModel->meshes[meshIndex].hasNormalMap )
			{
				// Default material for model with a normal map
				MaterialBump* newMat = new MaterialBump();
				newMat->setColorMap( glModel->meshes[meshIndex].texture );
				newMat->setNormalMap( glModel->meshes[meshIndex].normalmap );

				if( glModel->meshes[meshIndex].hasSpecularMap )
				{
					newMat->setSpecularMap( glModel->meshes[meshIndex].specularMap );
				}

				newMat->name = internModel->materials[ internModel->meshes[meshIndex].materialIndex ].name;
				materials.push_back( newMat );
			}
			else if( glModel->meshes[meshIndex].hasDiffuseMap )
			{
				MaterialPhongTexture* newMat = new MaterialPhongTexture();
				newMat->setTexture( glModel->meshes[meshIndex].texture );

				newMat->name = internModel->materials[ internModel->meshes[meshIndex].materialIndex ].name;
				materials.push_back( newMat );
			}
			else
			{
				MaterialDiffuse* newMat = new MaterialDiffuse();
				newMat->setDiffuse( vec4(0.3,0.3,1.0,1.0));
				newMat->name = internModel->materials[ internModel->meshes[meshIndex].materialIndex ].name;
				materials.push_back( newMat );
			}
		}
		else
		{
			MaterialPhong* newMat = new MaterialPhong();
			newMat->name = internModel->materials[ internModel->meshes[meshIndex].materialIndex ].name;
			materials.push_back( newMat );
		}
	}
}

void OpenGLStaticModelInstance::reloadGLData()
{
	glModel->reloadGLData();
}

void OpenGLStaticModelInstance::setMaterialDiffuseColor(const std::string& materialName, const vec4& color )
{
	for( unsigned int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			if( materials[i]->type == MATERIAL_TYPE::MATERIAL_BUMP )
			{
				((MaterialBump*)materials[i])->setColorMult( color );
			}
		}
	}
}

void OpenGLStaticModelInstance::setMaterialDiffuseColorMix(const std::string& materialName, const vec4& color1, const vec4& color2 )
{
	for( unsigned int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			if( materials[i]->type == MATERIAL_TYPE::MATERIAL_BUMP )
			{
				((MaterialBump*)materials[i])->setColorMult( color1 );
				((MaterialBump*)materials[i])->setColorMult2( color2 );
			}
		}
	}
}

void OpenGLStaticModelInstance::setMaterial( const std::string& materialName, Material* mat )
{
	bool found = false;
	for( int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			mat->name = materialName;
			delete materials[i];
			materials[i] = mat;
			found = true;
			break;
		}
	}
	assert(found);
}


TextureGL* OpenGLStaticModelInstance::getMaterialDiffuseTexture( const std::string& materialName )
{
	TextureGL* tex = NULL;
	for( int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			if( materials[i]->type == MATERIAL_TYPE::MATERIAL_BUMP )
			{
				tex = ((MaterialBump*)materials[i])->colorMap;
				break;
			}
			else if( materials[i]->type == MATERIAL_TYPE::MATERIAL_PHONG_TEXTURE )
			{
				tex = ((MaterialPhongTexture*)materials[i])->tex;
				break;
			}
		}
	}
	return tex;
}

void OpenGLStaticModelInstance::setMaterialDiffuseTexture( const std::string& materialName, TextureGL* diffuseTex )
{
	for( int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			if( materials[i]->type == MATERIAL_TYPE::MATERIAL_BUMP )
			{
				((MaterialBump*)materials[i])->setColorMap( diffuseTex );
				break;
			}
		}
	}
}

void OpenGLStaticModelInstance::setMaterialSpecularTexture( const std::string& materialName, TextureGL* specularTex )
{
	for( int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			if( materials[i]->type == MATERIAL_TYPE::MATERIAL_BUMP )
			{
				((MaterialBump*)materials[i])->setSpecularMap( specularTex );
			}
		}
	}
}

void OpenGLStaticModelInstance::setMaterialNormalTexture( const std::string& materialName, TextureGL* normalTex )
{
	for( int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			if( materials[i]->type == MATERIAL_TYPE::MATERIAL_BUMP )
			{
				((MaterialBump*)materials[i])->setNormalMap( normalTex );
			}
		}
	}
}

void OpenGLStaticModelInstance::update( float dt )
{
	if( animation != NULL )
	{
		animation->update(dt);

		meshAnimation->ComputeAnimation( animation->getAnimationIndex(), animation->getCurrentTime() );
		meshAnimation->skeletonLocal[0] = objectMatrix;
		meshAnimation->ComputeGlobalSkeleton();

	}
}

void OpenGLStaticModelInstance::Draw(MATERIAL_DRAW_PASS Pass)
{
	// issue one draw call for every submesh
	assert( internModel->meshes.size() == glModel->meshes.size() );// aieaieaie

	DrawCall drawcall;
	drawcall.Pass     = Pass;
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	for( auto it = meshesToDraw.begin(); it != meshesToDraw.end(); it++ )
	{
		int meshIndex = (*it);

		Shader* shader = materials[meshIndex]->getShader(Pass);
		if( shader != NULL )
		{
			drawcall.modelMat = objectMatrix;
			if( meshAnimation != NULL )
			{
				drawcall.modelMat = meshAnimation->skeletonGlobal[ internModel->meshes[meshIndex].nodeIndex ];
			}
			drawcall.modelMat *= sparePartMatrix[meshIndex];

			drawcall.material = materials[meshIndex];
			drawcall.mesh     = &(glModel->meshes[meshIndex]);
			drawcall.hasTransparency = glModel->meshes[meshIndex].isTranparent;
			drawcall.transparencyMode = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
			Renderer::get()->draw( drawcall );
		}
	}
}

//*********************************
// One instance of a skinned mesh
// (same vertices, different animation)
//
//*********************************

OpenGLSkinnedModelInstance::OpenGLSkinnedModelInstance( const mat4& mat, OpenGLModel * _glModel, const Model* _internModel ) : OpenGLModelInstance(mat, _glModel )
{
	internModel = _glModel->internModel;
	//glModel = dynamic_cast<OpenGLSkinnedModel*>(_glModel);
	glModel = (OpenGLSkinnedModel*)_glModel;
	assert(glModel != NULL );

	meshAnimation = new MeshAnimation( internModel );

	cout<<"Model bones = "<<internModel->skeleton.size()<<endl;
	cout<<"animation channels = "<<internModel->animations[0].channels.size()<<endl;

	if( internModel->animations.size() == 0 )
	{
		// just put him in bind pose :
		animation = new Animation(internModel, -1 );
	}
	else
	{
		animation = new Animation(internModel, 0 );
	}

	// shapekey test :
	shapekeyAnim = new MorphKeyAnimation();
	for( unsigned int meshIndex = 0; meshIndex < internModel->meshes.size(); meshIndex++ )
	{
		if( internModel->meshes[meshIndex].numMeshAnim > 0 )
		{
			if( glModel->meshes[meshIndex].numShapeKeys > 0 &&  glModel->meshes[meshIndex].shapekeys[0].hasPositions )
			{
				shapekeyAnim->init( &internModel->meshes[meshIndex], &glModel->meshes[meshIndex] );
				break;
			}
		}
	}

	for( unsigned int meshIndex = 0; meshIndex < internModel->meshes.size(); meshIndex++ )
	{
		SkinningShaderMaterial* newMat = new SkinningShaderMaterial();
		newMat->name = internModel->materials[ internModel->meshes[meshIndex].materialIndex ].name;
		cout<<"material name = "<<newMat->name<<endl;

		if( glModel->meshes[meshIndex].texture == NULL )
		{
			glModel->meshes[meshIndex].hasDiffuseMap = true;
			glModel->meshes[meshIndex].texture = TextureManager::get()->createRGBATexture( "dummy_texture", vec4(0.8,0.0,0.6,1.0), 64,64 );
		}

		newMat->setColorMap( glModel->meshes[meshIndex].texture );
		newMat->bonesMatrix = new mat4[ internModel->skeleton.size() ];
		for( unsigned int i=0; i<internModel->skeleton.size(); i++ )
		{
			newMat->bonesMatrix[i] = mat4(1.0);
		}
		newMat->nbBones = internModel->skeleton.size();

		if( glModel->meshes[meshIndex].hasNormalMap )
		{
			newMat->setNormalMap( glModel->meshes[meshIndex].normalmap );
		}

		if( glModel->meshes[meshIndex].hasSpecularMap )
		{
			newMat->setSpecularMap( glModel->meshes[meshIndex].specularMap );
		}

		materials.push_back( newMat );
	}
}

void OpenGLSkinnedModelInstance::reloadGLData()
{
	glModel->reloadGLData();
}

Material* OpenGLSkinnedModelInstance::getMaterial(const std::string& materialName)
{
	Material* material = NULL;
	for( int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			material = materials[i];
			break;
		}
	}
	return material;
}

void OpenGLSkinnedModelInstance::setMaterialDiffuseColorMix(const std::string& materialName, const vec4& color1, const vec4& color2 )
{
	for( unsigned int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			materials[i]->setColorMult( color1 );
			materials[i]->setColorMult2( color2 );
		}
	}
}

TextureGL* OpenGLSkinnedModelInstance::getMaterialDiffuseTexture( const std::string& materialName )
{
	TextureGL* tex = NULL;
	for( int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			tex = materials[i]->colorMap;
		}
	}
	return tex;
}

void OpenGLSkinnedModelInstance::setMaterialDiffuseTexture( const std::string& materialName, TextureGL* diffuseTex )
{
	for( int i=0; i< materials.size(); i++ )
	{
		if( materials[i]->name == materialName )
		{
			materials[i]->setColorMap( diffuseTex );
		}
	}
}

OpenGLSkinnedModelInstance::~OpenGLSkinnedModelInstance()
{
	for( int i=0; i< materials.size(); i++ )
	{
		delete materials[i];
	}
}

void OpenGLSkinnedModelInstance::Draw(MATERIAL_DRAW_PASS Pass)
{
	assert( internModel->meshes.size() == glModel->meshes.size() );// aieaieaie

	DrawCall drawcall;
	drawcall.Pass     = Pass;
	drawcall.modelMat = objectMatrix;
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	for( auto it = meshesToDraw.begin(); it != meshesToDraw.end(); it++ )
	{
		int meshIndex = (*it);
		if( ! internModel->meshes[meshIndex].hasBones )
		{
			continue;
		}

		Shader* shader = materials[meshIndex]->getShader(Pass);
		if( shader != NULL )
		{
			drawcall.material = materials[meshIndex];
			drawcall.mesh     = &(glModel->meshes[meshIndex]);
			drawcall.hasTransparency = glModel->meshes[meshIndex].hasDiffuseMap ? glModel->meshes[meshIndex].texture->getTransparency() : false;
			drawcall.transparencyMode = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
			Renderer::get()->draw( drawcall );
		}
	}
}

void OpenGLSkinnedModelInstance::update( float dt )
{
	if( true)//if visible
	{
		updateAnimation(dt);
		updateBoneMatrix();
	}
}

void OpenGLSkinnedModelInstance::updateAnimation( float dt )
{
	if( animation->getAnimationIndex() >= 0 )
	{
		animation->update(dt);
		meshAnimation->ComputeAnimation( animation->getAnimationIndex(), animation->getCurrentTime() );

		// update the shape keys animation :
		shapekeyAnim->update( animation->getCurrentTime() );

		meshAnimation->ComputeGlobalSkeleton();
	}
}

void OpenGLSkinnedModelInstance::updateBoneMatrix()
{
	for( int meshIndex = 0; meshIndex < internModel->meshes.size(); meshIndex++ )
	{
		for( int i=0; i< internModel->meshes[meshIndex].boneIndexOffset.size(); i++ )
		{
			int boneIndex = internModel->meshes[meshIndex].boneIndexOffset[i].first;
			mat4 bindPosition = internModel->meshes[meshIndex].boneIndexOffset[i].second;
			materials[meshIndex]->bonesMatrix[boneIndex] = meshAnimation->skeletonGlobal[boneIndex] * transpose(bindPosition);
		}
	}
}

int OpenGLSkinnedModelInstance::getBoneIndex( const std::string boneName )
{
	for( int i=0; i< internModel->skeleton.size(); i++ )
	{
		if( internModel->skeleton[i].name == boneName )
		{
			return i;
		}
	}
	return -1;
}


