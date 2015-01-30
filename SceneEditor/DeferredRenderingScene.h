#ifndef DEFERRED_SCENE_H
#define DIFERRED_SCENE_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>

#include "Scene.h"


class OpenGLStaticModelInstance;
class MaterialGlass;
class GLWidget;
class QITweakable;
class Skybox;
class SceneManager;
class FramebufferObject;
class Image;
class QOpenGLFramebufferObject;
class MaterialDeferred;
class Material;
class DynamicMesh;
class MaterialDeferredFullScreen;

class DeferredScene : public Scene
{
	Q_OBJECT

public:
	Skybox* skybox;

	OpenGLStaticModelInstance*  sphereInstance;
	OpenGLStaticModelInstance*  floorInstance;

	DeferredScene( SceneManager* manager );

	FramebufferObject* Gbuffer;
	QOpenGLFramebufferObject* GbufferQT;

	Image* GbufferViewer;
	Image* GbufferViewer2;

	MaterialDeferred* deferredMaterial;
	Material* sphereOriginalMaterial;
	MaterialDeferredFullScreen* fullscreenMaterial;

	DynamicMesh* fullscreenQuad;

	void init();
	void deinit();
	void update(float dt);
	void draw();
	void deferredPass();
	void fullscreenPass();
};

#endif
