#pragma once

#include <glm/glm.hpp>
#include <vector>
using namespace std;

class RayPicking
{
public:
	glm::vec3 rayStart; // pos de la camera ?
	glm::vec3 rayEnd;   // point "at"

	RayPicking(void);
	~RayPicking(void);

	void drawRay();
};
