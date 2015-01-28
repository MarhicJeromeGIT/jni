
#ifdef __ANDROID__

#else

#include "OBJLoader.h"
#include "string.h"
#include <sstream>

#include "macros.h"
#include <vector>
using namespace std;
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "TextureGL.h"
#include "Material.h"

using namespace glm;


namespace OBJ
{


bool loadOBJ( const char* filename, vector<vec3> *sommets, vector<vec3> *normales, vector<unsigned int> *triangles, vector<unsigned int> *indexNormales, vector<vec2>* coordUV, vector<unsigned int>* indexUV )
{
	cout<<"THIS FUNCTION IS DEPRECATED DO NOT USE"<<endl;
	return true;
}


WavefrontObject::WavefrontObject()
{
}

// read the ### header lines
bool WavefrontObject::ReadHeader( ifstream& instream, char* ptr )
{
	while( ptr[0] == '#' )
	{
		instream.getline (ptr, 1000 );
	}
	// next line should be 	mtllib clementine.mtl
	char mtl[30];
	int ok = sscanf(ptr,"mtllib %s",mtl);
	if( ok == 0 )
	{
		cout<<"couldn't read mtllib filename"<<endl;
		return false;
	}
	mtlfilename = string(mtl);
	//cout<<"mtlfile : "<<mtlfilename<<endl;
	return true; // ptr contains the next line
}

bool WavefrontObject::ReaddMesh( ifstream& instream, char* ptr, bool invertYZAxis )
{
	bool containsNormal = false;
	bool containsUV = false;

	//ptr should already contains the 'o object' line
	//o Untitled.005_clementine_eyes
	char name[100];
	int ok = sscanf(ptr,"o %s",name);
	bool isSubMaterial = false;
	if( ok != 1 )
	{
		if( sscanf(ptr, "usemtl") != -1 )
		{
			isSubMaterial = true;
			containsNormal = mesh->normales.size() != 0;
			containsUV = mesh->uvcoords.size() != 0;
			strcpy(name, "submaterial" );
		}
		else
		{
			cout<<"couldn't read the object name"<<endl;
			cout<<"got: "<<ptr<<" instead :/"<<endl;
			return false;
		}
	}
	if( !isSubMaterial )
	{
		cout<<"reading object "<<name<<endl;

		instream.getline (ptr, 1000 );
		// read the v 0.092340 3.681365 0.911923 lines:
		while( ptr[0] == 'v' && ptr[1] == ' ')
		{
			float x,y,z;
			if( invertYZAxis )
			{
				ok = sscanf(ptr, "v %f %f %f", &x, &z, &y);
				y = -y;
			}
			else
			{
				ok = sscanf(ptr, "v %f %f %f", &x, &y, &z);
			}
			if( ok != 3 )
			{
				cout<<"error reading line : "<<ptr<<endl;
				cout<<"v %f %f %f expected"<<endl;
			}

			mesh->vertices.push_back( vec3(x,y,z) );

			instream.getline (ptr, 1000 );

			// read the weights
			vector<int> boneIndex;
			vector<float> boneWeight;

			while( ptr[0] == 'w' && ptr[1] == ' ')
			{
				int boneId = 0;
				float weight = 0;
				ok = sscanf(ptr, "w %d %f", &boneId, &weight );
				if( ok != 2 )
				{
					cout<<"error reading line : "<<ptr<<endl;
					cout<<"w %d %f expected"<<endl;
				}

				boneIndex.push_back( boneId );
				boneWeight.push_back( weight );

				instream.getline (ptr, 1000 );
			}

			// fait la moyenne:
			float sum = 0;
			for( int i=0;i<boneWeight.size(); i++ )
				sum += boneWeight[i];
			if( sum > 0.01 )
			{
				for( unsigned int i=0;i<boneWeight.size(); i++ )
				{
					boneWeight[i] = boneWeight[i] / sum;
				}
			}

			mesh->boneIndex.push_back( boneIndex );
			mesh->boneWeight.push_back( boneWeight );
		}

		//read the text coord vt 0.736690 0.063880
		while( ptr[0] == 'v' && ptr[1] == 't')
		{
			float s,t;
			ok = sscanf(ptr, "vt %f %f", &s, &t);
			if( ok != 2 )
			{
				cout<<"error reading line : "<<ptr<<endl;
				cout<<"vt %f %f expected"<<endl;
			}
			containsUV = true;
			mesh->uvcoords.push_back( vec2(s,t) );
			instream.getline (ptr, 1000 );
		}
		//read the normals vn 0.624043 0.767663 0.145604
		while( ptr[0] == 'v' && ptr[1] == 'n')
		{
			float x,y,z;
			if( invertYZAxis )
			{
				ok = sscanf(ptr, "vn %f %f %f", &x, &z, &y);
				y = -y;
			}
			else
			{
				ok = sscanf(ptr, "vn %f %f %f", &x, &y, &z);
			}
			if( ok != 3 )
			{
				cout<<"error reading line : "<<ptr<<endl;
				cout<<"vn %f %f %f expected"<<endl;
			}
			containsNormal = true;
			mesh->normales.push_back( vec3(x,y,z) );
			instream.getline (ptr, 1000 );
		}
	}

	BaseMesh::submesh* newSubmesh = new BaseMesh::submesh();
	strcpy( newSubmesh->SubMeshName, name);

	// material :
	char materialName[50];
	if( sscanf( ptr, "usemtl %s", materialName ) != 0 )
	{
		instream.getline (ptr, 1000 );
		cout<<"material name : "<<materialName<<endl;
		strcpy( newSubmesh->MaterialName, materialName );
	}
	//s 1
	if( ptr[0] == 's' ) // I have no idea what that is
	{
		instream.getline (ptr, 1000 );
	}

	// list of faces : f 1/1/1 2/2/2 3/3/3
	while( ptr[0] == 'f' )
	{
		int v[3]  = {0,0,0};
		int uv[3] = {0,0,0};
		int n[3]  = {0,0,0};

		if( containsNormal && containsUV )
		{
			// style 1/1/1:
			int ok = sscanf( ptr, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v[0], &uv[0], &n[0],
																  &v[1], &uv[1], &n[1],
																  &v[2], &uv[2], &n[2] );
			if( ok != 9 )
			{
				cout<<"expected f 1/1/1 2/2/2 3/3/3, got "<<ptr<<endl;
			}
		}
		else if( containsNormal && !containsUV )
		{
			// style 1//1:
			int ok = sscanf( ptr, "f %d//%d %d//%d %d//%d", &v[0], &n[0],
														    &v[1], &n[1],
															&v[2], &n[2] );
			if( ok != 6 )
			{
				cout<<"expected f 1//1 2//2 3//3, got "<<ptr<<endl;
			}
		}
		else if( !containsNormal && containsUV )
		{
			// style 1/1
			int ok = sscanf( ptr, "f %d/%d %d/%d %d/%d", &v[0], &uv[0],
															&v[1], &uv[1],
															&v[2], &uv[2] );
			if( ok != 6 )
			{
				cout<<"expected f 1/1 2/2 3/3, got "<<ptr<<endl;
			}
		}
		else // ni uv ni normales
		{
			int ok = sscanf( ptr, "f %d %d %d", &v[0], &v[1], &v[2] );
			if( ok != 3 )
			{
				cout<<"expected f 1 2 3, got "<<ptr<<endl;
			}
		}

		// this awesome format numerotes from index 1 :/
		newSubmesh->vertex_indices.push_back( v[0] -1);
		newSubmesh->vertex_indices.push_back( v[1] -1);
		newSubmesh->vertex_indices.push_back( v[2] -1);

		if( containsNormal )
		{
			newSubmesh->normales_indices.push_back( n[0] -1);
			newSubmesh->normales_indices.push_back( n[1] -1);
			newSubmesh->normales_indices.push_back( n[2] -1);
		}

		if( containsUV )
		{
			newSubmesh->uvcoords_indices.push_back( uv[0] -1);
			newSubmesh->uvcoords_indices.push_back( uv[1] -1);
			newSubmesh->uvcoords_indices.push_back( uv[2] -1);
		}

		instream.getline (ptr, 1000 );
	}

	cout<<"Contains "<<newSubmesh->vertex_indices.size()/3<<" triangles"<<endl;
	mesh->submeshes.push_back( newSubmesh );
	return true;
}

bool alpha_sort (BaseMesh::submesh* first, BaseMesh::submesh* second)
{
	return true;
	/*
	if( second->mat->hasAlpha )
		return true;
	else if( !first->mat->hasAlpha)
		return true;
	return false;
	*/
}

bool WavefrontObject::LoadObject( const string& directory, const string& filename, bool invertYZAxis /*= false*/ )
{
	return false;

/*	//vector< Mesh* > meshes;
	//vector< Material* > materials;
	ifstream instream;
	string fullpath = directory + filename;
	instream.open ( fullpath.c_str(), ifstream::in );
	if( ! instream.good() )
	{
		cout<<"couldn't open "<<fullpath<<endl;
		return false;
	}

	char ptr[1000];
	instream.getline (ptr, 1000);

	ReadHeader(instream,ptr);

	instream.getline (ptr, 1000); // readHeader stops after reading the mtlline
	mesh = new BaseMesh();

	while( !instream.eof() )
	{
		bool ok = ReaddMesh(instream,ptr, invertYZAxis);
		if( !ok )
		{
			cout<<"Error reading objects at line"<<endl;
			cout<<ptr<<endl;
			return false;
		}
	}
	instream.close();

	// read the mtl file:
	fullpath = directory + mtlfilename;
	instream.open ( fullpath.c_str(), ifstream::in );
	if( ! instream.good() )
	{
		cout<<"couldn't open "<<fullpath<<endl;
		return false;
	}
	instream.getline (ptr, 1000);
	int materialCount = 0;
	while( ptr[0] == '#' )
	{
		instream.getline (ptr, 1000);
		// # Material Count: 7
		int count = -1;
		if( sscanf( ptr, "# Material Count: %d", &count ) == 1)
		{
			materialCount = count;
		}
	}
	cout<<"contains "<<materialCount<<" materials"<<endl;

	for( int i=0; i<materialCount; i++ )
	{
		// skip empty lines
		while( strlen(ptr) == 0 )
			instream.getline (ptr, 1000);

		// newmtl clementine_body
		char matName[50];
		if( sscanf( ptr, "newmtl %s", matName ) != 1)
		{
			cout<<"Erreur reading material"<<endl;
		}

		vec3 Kd = vec3(0.0);
		char diffuse_texture[50];
		bool hasAlpha = false;
		bool hasTexture = false;
		// read all the attributes, in any order (what a format...)
		while( strlen(ptr) != 0 )
		{
			char map_Kd[50];
			if( sscanf( ptr, "map_Kd %s", map_Kd ) == 1 )
			{
				hasTexture = true;
				strcpy( diffuse_texture, map_Kd );
			}

			char alphaMap[50];
			if( sscanf( ptr, "map_d %s", alphaMap ) != 0 )
			{
				hasAlpha = true;
			}

			if( sscanf( ptr, "Kd" ) != -1 )
			{
				sscanf( ptr, "Kd %f %f %f", &Kd.x, &Kd.y, &Kd.z );
			}

			instream.getline (ptr, 1000);
		}

		MaterialTexture* newMat  = new MaterialTexture();
		newMat->materialName     = string(matName);
		newMat->textureDirectory = directory;
		newMat->textureName      = string(diffuse_texture);
		newMat->hasAlpha         = hasAlpha;
		newMat->hasTexture       = hasTexture;
		//newMat->diffuse_color    = Kd;
		if( newMat->hasTexture )
		{
			TextureGL* tex = new TextureGL( newMat->textureDirectory + newMat->textureName );
			if( tex->error )
			{
				cout<<"couldn't load texture "<<newMat->textureDirectory + newMat->textureName<<endl;
				tex = NULL;
				// setup a diffuse color :
				newMat->hasTexture = false;
				//newMat->diffuse_color = vec3(1,1,1);
			}
			newMat->tex = tex;
		}
		else
		{
			newMat->tex = NULL;
		}

		materialMap.insert( pair<string,Material*>( newMat->materialName, newMat ) );

		// look for all the submeshes using this material :
		for( auto m= mesh->submeshes.begin(); m != mesh->submeshes.end(); m++ )
		{
			BaseMesh::submesh* sub = (*m);
			const char* submatName = sub->MaterialName;
			if( strcmp( submatName, matName) == 0 )
			{
				sub->mat = newMat;
			}
		}
	}

	// we will change the draw order so that all meshes using an 'alpha' material will be drawn last:
	mesh->submeshes.sort( alpha_sort );

	return true;*/
}


}

#endif
