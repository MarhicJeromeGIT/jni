#pragma once

/*
#ifdef __ANDROID__
	#include <stl/config/_android.h>
#endif
*/
#include <string>
class Shader;
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <list>

class TextureGL;
#include <iostream>

class Material;
class Mesh;

class Mesh
{
public:
	std::string name;
public:
	Mesh() : name("") {}
	Mesh(const std::string& name);
	~Mesh(void);

	const std::string& getName(){ return name; }
};

// a list of triangles + a name
class BaseMesh : public Mesh
{
public:
	vector<glm::vec3> vertices;
	vector<glm::vec3> normales;
	vector<glm::vec2> uvcoords;

	// for skeletal animation
	vector< vector<int> > boneIndex; // up to 4 bones per vertice
	vector< vector<float> > boneWeight;

	class submesh
	{
	public:
		char SubMeshName[50];

		vector<unsigned int> vertex_indices;
		vector<unsigned int> normales_indices;
		vector<unsigned int> uvcoords_indices;

		vector<glm::vec3> normales;

		char MaterialName[50];
		Material* mat;

		submesh()
		{
		}
	};
	list<submesh*> submeshes;

public:

	BaseMesh() : Mesh("") {}
	BaseMesh( const std::string& name ) : Mesh(name) {}

	submesh* findSubmesh( const string& submeshName )
	{
		for( list<submesh*>::iterator it = submeshes.begin(); it != submeshes.end(); it++ )
		{
			if( string( (*it)->SubMeshName ) ==  submeshName )
			{
				return *it;
			}
		}
		cout<<"Couldn't find submesh "<<submeshName<<endl;
		return NULL;
	}
};
