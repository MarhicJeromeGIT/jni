#ifndef DEFERRED_SCENE_H
#define DIFERRED_SCENE_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>
#include <vector>
#include "Scene.h"
#include <glm\glm.hpp>

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
class MaterialGUI;

struct lightInfo
{
	glm::vec4 Position;
	glm::vec4 Color;
	MaterialDeferredFullScreen* fullscreenMaterial;
};

class DeferredScene : public Scene
{
	Q_OBJECT

public:
	Skybox* skybox;

	float lightDistance;
	float lightAngle;

	OpenGLStaticModelInstance*  sphereInstance;
	OpenGLStaticModelInstance*  floorInstance;
	OpenGLStaticModelInstance*  lightInstance;

	DeferredScene( SceneManager* manager );

	FramebufferObject* Gbuffer;
	FramebufferObject* LightBuffer;
	QOpenGLFramebufferObject* GbufferQT;

	Image* GbufferViewer;
	Image* GbufferViewer2;
	
	MaterialGUI* fullscreenDiffuseMaterial;

	DynamicMesh* fullscreenQuad;

	std::vector<lightInfo> lightInfoVector;

	void init();
	void deinit();
	void update(float dt);
	void draw();
	void deferredPass();
	void fullscreenPass();
};

#endif
