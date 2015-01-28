#ifndef OVERLAY_H
#define OVERLAY_H

#include "Material.h"
#include <glm/glm.hpp>
#include <vector>
using namespace glm;
#include "OpenGLModel.h"

// not sure what to do with that... maybe just a generic widget class

class Overlay
{
public:
	Overlay();
	virtual ~Overlay(){}

	virtual void update(float dt);
	virtual void Draw(MATERIAL_DRAW_PASS Pass);
};



#endif
