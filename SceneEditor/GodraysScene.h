#ifndef GODRAY_SCENE_H
#define GODRAY_SCENE_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>

#include "Scene.h"


class OpenGLStaticModelInstance;
class MaterialGlass;
class QITweakable;
class Skybox;
class OpenGLSkinnedModelInstance;
class Image;
class DynamicMesh;
class GodrayShader;

class GodrayScene : public Scene
{
	Q_OBJECT

public:
	DynamicMesh* fullscreenQuad;
	GodrayShader* godrayShader;

	Skybox* skybox;
	OpenGLSkinnedModelInstance* animatedModel;
	OpenGLStaticModelInstance* lightModel;
	Image* fboImage;

	GodrayScene(SceneManager* manager);

	float decay;
	float weight;
	float exposure;

	void init();
	void deinit();
	void update(float dt);
	void draw();

private:
	void drawFBO();
	void drawLightShaft();
};

#endif
