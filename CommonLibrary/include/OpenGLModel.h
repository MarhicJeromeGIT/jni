#pragma once

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include "Mesh.h"
#include "Model.h"
#include <MaterialDrawPass.h>
#include "Renderer.h"
#include "AABB.h"
#include <queue>
using namespace std;

class TextureGL;
class SkinningShader;
class DiffuseShader;
class SkinningShaderMaterial;
class MonsterPath;
class MeshAnimation;
class MorphKeyAnimation;
class Animation;

struct ShapeKeyBuffers
{
	bool hasPositions;
	GLuint vertexBuffer;
	bool hasNormal;
	GLuint normalBuffer;
};

class InternalMesh : public Mesh
{
public:
	InternalMesh();
	~InternalMesh();

	void releaseGLData();

	int totalVertices;
	int nbTriangles;
	
	bool hasAnimations;
	bool hasBones;
	bool hasNormals;
	bool hasUVs;

	GLuint vertexBuffer;
	GLuint bonesIndexBuffer;
	GLuint bonesWeightBuffer;
	GLuint normalBuffer;
	GLuint tangenteBuffer;
	GLuint bitangenteBuffer;

	GLuint texCoordBuffer;

	GLuint textureWeightBuffer;
	bool   hasSecondUVSet;
	GLuint texCoordBuffer2;

	GLuint texCoordArrayBuffer; // tex with 3 coordinates xyz

	int indicesStart;
	GLuint indicesBuffer;

	GLuint speedBuffer;

	bool hasDiffuseMap;
	TextureGL* texture;
	bool hasNormalMap;
	TextureGL* normalmap;
	bool hasSpecularMap;
	TextureGL* specularMap;

	bool isTranparent;

	// for morphing :
	GLuint vertexBufferMorph;
	GLuint normalBufferMorph;

	// per-vertex color :
	GLuint colorBuffer;

	// per-vertex transparency :
	GLuint transparencyBuffer;

	// shapekeys
	int numShapeKeys;
	ShapeKeyBuffers* shapekeys;

	// Instancing stuff
	int nbInstances;
	GLuint modelMatrixBuffer;

};

class DynamicMesh : public Mesh
{
public:
	InternalMesh mesh;
	int maxVertices;
	int maxTriangles;

	DynamicMesh(int maxVertices, int maxTriangles);
	~DynamicMesh();

	void updateGeometry( glm::vec3* vertices, glm::vec2* texCoord, int nbVertices, unsigned int* triangles, int nbTriangles  );
	void updateGeometry( glm::vec3* vertices, glm::vec2* texCoord, int nbVertices, unsigned short* triangles, int nbTriangles  );
	void updateGeometry( glm::vec3* vertices, glm::vec3* texCoord, int nbVertices, unsigned short* triangles, int nbTriangles  );

	template<class T>
	static void updateBuffer( T* data, int nbElements, GLuint& buffer, bool create = false )
	{
		if( !glIsBuffer( buffer ) )
		{
			glGenBuffers(1,&buffer);
		}
		glBindBuffer( GL_ARRAY_BUFFER, buffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(T) * nbElements, data, GL_DYNAMIC_DRAW );
	}
};

class OpenGLModel : public Mesh
{
public:
	const Model* internModel; // au cas ou
	vector<InternalMesh> meshes;

	void reloadGLData();

public:
	OpenGLModel( const Model* internModel );
	virtual ~OpenGLModel(void);

	virtual void update( float dt );

private:
	bool loadMesh( const Model::Mesh* mesh );
};

class OpenGLStaticModel : public OpenGLModel
{
public:
	OpenGLStaticModel( const Model* internModel );
	virtual ~OpenGLStaticModel(){}
};

class OpenGLSkinnedModel : public OpenGLModel
{
public:
	OpenGLSkinnedModel( const Model* internModel );
	virtual ~OpenGLSkinnedModel(){}
};


//***************************************//
// Parent class for model instances
//
//***************************************//
class OpenGLModelInstance
{
public:
	AABB* boundingBox;
	glm::mat4 objectMatrix;
	const Model* internModel;
	OpenGLModel* glModel;
	glm::vec3 position;
	list<unsigned int> meshesToDraw; // maybe we don't want to draw all our meshes
	

	OpenGLModelInstance( const glm::mat4& objectMatrix, OpenGLModel* glModel );
	virtual ~OpenGLModelInstance();

	virtual void update( float dt ) = 0;
	virtual void Draw(MATERIAL_DRAW_PASS Pass) = 0;

	void SetPosition( const glm::vec3& position );
	glm::vec3 getPosition(){ return glm::vec3( objectMatrix[3][0], objectMatrix[3][1], objectMatrix[3][2] ); }

	bool intersectRay( glm::vec3& pos, const glm::vec3& rayStart, const glm::vec3& rayDir );


	virtual void reloadGLData() = 0;

	int getMeshIndex( const char* meshName );
};

//***************************************//
// Static model instance
//
//***************************************//
class OpenGLStaticModelInstance : public OpenGLModelInstance
{
public:
	vector<Material*> materials; // un material par mesh
	vector<glm::mat4> sparePartMatrix;

	Animation* animation;
	MeshAnimation* meshAnimation;

	OpenGLStaticModelInstance( const glm::mat4& objectMatrix, OpenGLModel* glModel, const Model* internModel = NULL );
	virtual ~OpenGLStaticModelInstance(){}

	virtual void update( float dt );
	virtual void Draw(MATERIAL_DRAW_PASS Pass);

	virtual void reloadGLData();

	void setMaterialDiffuseColor(const std::string& materialName, const glm::vec4& color );
	void setMaterialDiffuseColorMix(const std::string& materialName, const glm::vec4& color1, const glm::vec4& color2 );
	void setMaterial( const std::string& materialName, Material* mat );
	void setMaterial( int materialIndex, Material* mat );
	TextureGL* getMaterialDiffuseTexture( const std::string& materialName ); // houlala
	void setMaterialDiffuseTexture( const std::string& materialName, TextureGL* diffuseTex );
	void setMaterialSpecularTexture( const std::string& materialName, TextureGL* diffuseTex );
	void setMaterialNormalTexture( const std::string& materialName, TextureGL* normalTex );
};


//***************************************//
// Skinned model instance
//
//***************************************//

class OpenGLSkinnedModelInstance : public OpenGLModelInstance
{
public:
	Animation* animation;

	MeshAnimation* meshAnimation;

	// Just one morph key for now
	MorphKeyAnimation* shapekeyAnim;

	vector<SkinningShaderMaterial*> materials;

	OpenGLSkinnedModelInstance( const glm::mat4& objectMatrix, OpenGLModel* glModel, const Model* internModel = NULL );
	virtual ~OpenGLSkinnedModelInstance();

	Material* getMaterial(const std::string& materialName);
	void setMaterialDiffuseColorMix(const std::string& materialName, const glm::vec4& color1, const glm::vec4& color2 );
	TextureGL* getMaterialDiffuseTexture( const std::string& materialName ); // houlala
	void setMaterialDiffuseTexture( const std::string& materialName, TextureGL* diffuseTex );

	virtual void update( float dt );
	void updateBoneMatrix();
	virtual void Draw(MATERIAL_DRAW_PASS Pass);

	virtual void reloadGLData();

public:

	int getBoneIndex( const std::string boneName ); //return -1 if not found

private:
	void updateAnimation( float dt );
	void updateSkeletonWithAnimation( int animationIndex, float animationTime, float animationWeight, bool blendingAnim = false );
};

//
