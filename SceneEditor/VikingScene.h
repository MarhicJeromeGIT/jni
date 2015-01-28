#ifndef VIKING_SCENE_H
#define VIKING_SCENE_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>
#include <glm/glm.hpp>
#include "Scene.h"
#include <vector>

class OpenGLStaticModelInstance;
class MaterialGlass;
class QITweakable;
class Skybox;
class OpenGLSkinnedModelInstance;
class Image;
class DynamicMesh;
class GodrayShader;
class MaterialPhong;
class FramebufferObject;
class QOpenGLFramebufferObject;
class MaterialGrass;
class MaterialPhongTexture;

class VikingScene : public Scene
{
	Q_OBJECT

public:

	FramebufferObject* fbo;
	QOpenGLFramebufferObject* fbo2;
	static const int fboN = 512;

	Skybox* skybox;
	OpenGLSkinnedModelInstance* animatedModel;
	OpenGLStaticModelInstance* lightModel;
	Image* shadowProjImage;
	Image* shadowImage;
	bool drawTextures;

	bool lightPosChanged;

	OpenGLStaticModelInstance* vikingInstance;
	int viking_mesh_index;

	std::vector<glm::vec3> clusterPosition;
	void sort_indexes( std::vector<int>& indexes );

	VikingScene(SceneManager* manager);

	void init();
	void deinit();
	void update(float dt);
	void draw();

private:
	void drawGrass();
};

#endif
