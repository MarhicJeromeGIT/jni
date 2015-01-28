#pragma once
/*
#ifdef __ANDROID__
	#include <stl/config/_android.h>
#endif*/

#include <glm/glm.hpp>
#include <vector>
using namespace std;

class Bone
{
public:
  char name[30];

  Bone* parent;

  unsigned int BoneIndex;

  glm::vec3 color;

  // relative to the parent
  glm::vec3 offset;
  glm::vec3 rotation;

  // relative position of the end position wrt the root
  glm::vec4 position;
  glm::mat4 localTransform;
  glm::mat4 worldTransform;

	glm::mat4 Binv;
	glm::mat4 ASSIMP_Binv;

  Bone( const char* name, Bone* parent, const glm::vec3& offset, const glm::vec3& rotation);
	Bone( Bone* p, const glm::mat4& transform);

	Bone* findBoneByName(const char* n);
	Bone* findBoneByIndex(unsigned int index);
  void draw();
	void computeBinv();
	void computeWorldTransform();
	void setBonePosition( glm::vec3 pos );

	void recomputeWorldTransform();
private:
   void addChild( Bone* child );
   vector<Bone*> childs;
};
