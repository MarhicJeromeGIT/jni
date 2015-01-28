#pragma once

#ifdef __ANDROID__

#else

#include <GL/glew.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
using namespace std;
#include <map>

class Material;
class TextureGL;
class BaseMesh;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace OBJ
{

// 3 points par triangle
bool loadOBJ( const char* filename, vector<glm::vec3>* sommets, vector<glm::vec3>* normales, vector<unsigned int>* triangles, vector<unsigned int> *indexNormales, vector<glm::vec2>* coordUV, vector<unsigned int>* indexUV );


class WavefrontObject
{
public:
	// Il est tout pourri ce format .obj
	BaseMesh* mesh;
	map<string,Material*> materialMap;

	string mtlfilename;

public:
	WavefrontObject();
	bool LoadObject( const string& directory, const string& filename, bool invertYZAxis = false );



private:
	bool ReadHeader( ifstream& instream, char* ptr );
	bool ReaddMesh( ifstream& instream, char* ptr, bool invertYZAxis );
};

}

#endif

