#pragma once

/*
#ifdef __ANDROID__
#include <stl/config/_android.h>
#endif
*/

#include <string>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class AAsset;


#ifdef __ANDROID__
#define FILESTREAM AAsset
#else
#define FILESTREAM FILE
#endif

#include "Quaternion.h"

struct aiScene;

class Model
{
	friend class ModelImport;
public:
	string modelName;

	class Bone;
	class Mesh;
	class Material;
	class MorphAnimation;
	class BoneAnimation;
	class ShapeKey;

public:
	// animation data:
	vector<Bone> skeleton;
	vector<Mesh> meshes;
	vector<Material> materials;
	vector<BoneAnimation> animations;

public:
	Model();

	bool Load( const string& filename );
	~Model(void);

#ifndef __ANDROID__
	bool testImportExport( const string& filename );
	bool Save( const string& filename );
#endif
};

class Model::Bone
{
	friend class ModelImport;
public:
	string name;

	glm::mat4 localTransform;
	glm::mat4 worldTransform;

	vector<int> childsIndex; // childs index in the skeleton array
	int parentIndex; // -1 => root

	bool read( FILESTREAM* fp );
#ifndef __ANDROID__
	bool write( FILE* fp );
#endif
};



struct BoneAnimationChannel
{
	struct LocRotScale;

	int boneIndex;
	vector< LocRotScale > locrotscale;

	bool read( FILESTREAM* fp );
#ifndef __ANDROID__
	bool write( FILE* fp );
#endif
};

struct BoneAnimationChannel::LocRotScale
{
	float time;
	Quaternion      rotation;
	glm::vec3       position;
	glm::vec3       scale;
};

class Model::BoneAnimation
{
	friend class ModelImport;
public:
	float duration; // en secondes
	float ticksPerSecond;
	vector< BoneAnimationChannel > channels;

	bool read( FILESTREAM* fp );
#ifndef __ANDROID__
	bool write( FILE* fp );
#endif
};

class Model::MorphAnimation
{
public:
	MorphAnimation();
	float maxTime;
	vector< pair<float, float> > morphTimeline;

	bool read( FILESTREAM* fp );
#ifndef __ANDROID__
	bool write( FILE* fp );
#endif
};

class Model::ShapeKey
{
	friend class ModelImport;

public:
	ShapeKey();
	string name; // important !

	MorphAnimation animation;

	int   nbVertices; // same has Mesh::nbVertices
	bool  hasPositions;
	bool  hasNormales;
	bool  hasTexCoords;
	bool  hasTangentes;
	bool  hasBitangentes;
	glm::vec3* vertex_meshanim_positions;
	glm::vec3* vertex_meshanim_normales;
	glm::vec2* vertex_meshanim_texcoords;
	glm::vec3* vertex_meshanim_tangentes;
	glm::vec3* vertex_meshanim_bitangentes;

	bool read( FILESTREAM* fp );
#ifndef __ANDROID__
	bool write( FILE* fp );
#endif
};

class Model::Mesh
{
	friend class ModelImport;
public:
	string name; // meshes of the same model are drawn in alphabetical order for transparency purposes.

	// each mesh has a bone offset
	vector< pair<int, glm::mat4> > boneIndexOffset;

	// those should have the same size
	int nbVertices;
	glm::vec3* vertex_positions;
	glm::vec3* vertex_normales;
	glm::vec3* vertex_tangentes;
	glm::vec3* vertex_bitangentes;

	glm::vec2* vertex_uvs;

	bool hasSecondUVSet;
	glm::vec2* vertex_uvs2;

	bool hasBones;
	glm::vec4* bonesWeight;
	glm::ivec4* bonesIndex;

	unsigned int materialIndex;

	// some meshes are just lines :
	vector<glm::ivec2> lineIndices;

	// The shapekeys
	int numMeshAnim;
	ShapeKey* shapekeys;

	// triangulated
	int nbTriangles;
	glm::ivec3* indices; // vec3 -> un triangle

	// animation...
	int nodeIndex; // just one 'bone', no weight

public:

	bool read( FILESTREAM* fp );
#ifndef __ANDROID__
	bool write( FILE* fp );
#endif
};

class Model::Material
{
public:
	string name;
	bool hasDiffuseTexture;
	string diffuse_texture;
	bool hasNormalTexture;
	string normal_texture;
	bool hasSpecularTexture;
	string specular_texture;

public:

	bool read( FILESTREAM* fp );
#ifndef __ANDROID__
	bool write( FILE* fp );
#endif
};

