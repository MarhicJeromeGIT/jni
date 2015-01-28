#ifndef INSTANCING_SCENE_H
#define INSTANCING_SCENE_H

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
class MaterialInstanceStatic;

class InstancingScene : public Scene
{
	Q_OBJECT

public:

	Skybox* skybox;

	OpenGLStaticModelInstance* fenceInstance;
	MaterialInstanceStatic* instanceMat;

	InstancingScene(SceneManager* manager);

	void init();
	void deinit();
	void update(float dt);
	void draw();
};

#endif
