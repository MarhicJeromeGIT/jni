#ifndef _LEVEL_H_
	#define _LEVEL_H_

#include <glm/glm.hpp>
#include <string>
#include <vector>
using namespace std;

class StaticMesh;
class TextureGL;

struct LevelObject
{
	string groupName;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	bool animated;
};

struct LevelPath
{
	string pathName;
};

struct LevelHeightmap
{
	string groupName;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

// test class to load a custom level file (.map)
class Level
{
public:
	bool isLoaded;
	
	vector<LevelObject> objects;
	vector<LevelPath> paths;
	vector<LevelHeightmap> heightmaps;

public:
	Level();
	Level(const string& filename);
	bool Load(const string& filename);

};

#endif

