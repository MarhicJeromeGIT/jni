#ifndef _RENDERER_
#define _RENDERER_

#include <list>
using namespace std;
#include "Material.h"

class Mesh;

enum TRANSPARENCY_MODE
{
	GL_ONE_GL_ZERO, // that's the default
	GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA, // (smoke, glass, grass) (semitransparent ?)
	GL_SRC_ALPHA_GL_ONE, // fire, magic, glow, particles (edges are completely transparent ?)
	GL_ONE_GL_ONE, // bloom ? (accumulate color)
	GL_SRC_ALPHA_GL_ONE_MINUS_DST_ALPHA,
};

struct DrawCall
{
public:

	DrawCall()
	{
		disableDepthWrite = false;
		hasTransparency   = false;
		transparencyMode  = GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
	}

	glm::mat4 modelMat;
	glm::mat4 viewMat;
	glm::mat4 projMat;

	Mesh* mesh;
	Material* material;
	MATERIAL_DRAW_PASS Pass;

	// transparency stuff :
	bool hasTransparency;
	TRANSPARENCY_MODE transparencyMode;

	// depth
	bool disableDepthWrite;

};


class Renderer
{
private:
	Renderer();
	static Renderer* thisObject;
	
	vector<DrawCall> drawcalls;
public:
	static Renderer* get();
	void beginFrame(); // should lock
	
	void draw( const DrawCall& drawcall );
	
	void endFrame(); // unlock + sort the drawcalls + send drawcalls to the GPU
	
	void clearFrame(){ drawcalls.clear(); }
private:
	void sortDrawCalls();
};

#endif
