#ifndef GRASS_SCENE_H
#define GRASS_SCENE_H

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

class GrassScene : public Scene
{
	Q_OBJECT

public:

	FramebufferObject* fbo;
	QOpenGLFramebufferObject* fbo2;
	static const int fboN = 512;

	DynamicMesh* fullscreenQuad;
	MaterialPhongTexture* phongMaterial;

	MaterialGrass* grassMaterial;

	DynamicMesh* plane;
	DynamicMesh* grassBlades;

	Skybox* skybox;
	OpenGLSkinnedModelInstance* animatedModel;
	OpenGLStaticModelInstance* lightModel;
	Image* shadowProjImage;
	Image* shadowImage;
	bool drawTextures;

	bool lightPosChanged;

	OpenGLStaticModelInstance* treeInstance;
	OpenGLStaticModelInstance* fenceInstance;

	std::vector<glm::vec3> clusterPosition;
	void sort_indexes( std::vector<int>& indexes );

	GrassScene(SceneManager* manager);

	void init();
	void deinit();
	void update(float dt);
	void draw();
	void shadowMapPass();

private:
	void drawGrass();
};

#endif
