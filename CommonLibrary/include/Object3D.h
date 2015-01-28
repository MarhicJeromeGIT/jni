#pragma once

#include <glm/glm.hpp>
#include "Material.h"

class Mesh;


class Object3D
{
public:
	Mesh* mesh;
	Material* material;

	glm::mat4 objectMatrix;

	Object3D();
	void Draw( MATERIAL_DRAW_PASS Pass );
};
