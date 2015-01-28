#ifndef TESSELATION_SCENE_H
#define TESSELATION_SCENE_H

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
class TerrainTesselationShader;
class MaterialTerrainTesselation;

class TesselationScene : public Scene
{
	Q_OBJECT

public:

	Skybox* skybox;

	MaterialTerrainTesselation* terrainMat;
	DynamicMesh* terrainMesh;

	TesselationScene(SceneManager* manager);

	void init();
	void deinit();
	void update(float dt);
	void draw();
};

#endif
