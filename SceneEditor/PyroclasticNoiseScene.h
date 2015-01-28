#ifndef PYROCLASTIC_SCENE_H
#define PYROCLASTIC_SCENE_H

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
class UVShader;
class RaycastShader;

class PyroclasticNoiseScene : public Scene
{
	Q_OBJECT

public:
	
	Image* NoiseImage;
	Image* BackCubeImage;

	Skybox* skybox;

	UVShader* uvshader;
	RaycastShader* raycastShader;

	PyroclasticNoiseScene(SceneManager* manager);

	void init();
	void deinit();
	void update(float dt);
	void draw();

	void drawCube();
};

#endif
