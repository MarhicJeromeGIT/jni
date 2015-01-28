#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

class Model;

// Axis aligned bounding box
class AABB
{
public:
	glm::vec3 minPoint; //in model space
	glm::vec3 maxPoint;


public:
	AABB( glm::mat4& objectMat, const Model* model);

	void Draw(glm::mat4& objectMat );

	bool rayIntersection( const glm::mat4& objectMat, const glm::vec3& p0, const glm::vec3&  p1  );
};


#endif
