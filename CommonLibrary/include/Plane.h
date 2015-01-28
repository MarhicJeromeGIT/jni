#pragma once

#include <glm/glm.hpp>

class Plane
{
public:
	// 	Ax + By + Cz + D = 0
	glm::vec3 ABC;
	float D;

	glm::vec3 point;

	// la normale est n = cross(p1-p0, p2-p0)
	Plane(){}
	Plane( const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);
	Plane( const glm::vec3& normal, const glm::vec3& p0 );
	~Plane(void);

	float Distance( const glm::vec3& point )
	{
		return glm::dot(ABC,point) + D;
	}

};



