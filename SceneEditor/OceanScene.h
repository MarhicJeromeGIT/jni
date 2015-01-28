#ifndef OCEAN_SCENE_H
#define OCEAN_SCENE_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>

#include "Scene.h"
#include "fftw3.h"


class OpenGLStaticModelInstance;
class MaterialGlass;
class GLWidget;
class QITweakable;
class Skybox;
class SceneManager;
class DynamicMesh;
class MaterialOcean;
class MaterialPhongTexture;
class Image;
class OceanGeometryGeneratorShader;

class OceanScene : public Scene
{
	Q_OBJECT

public:
	Skybox* skybox;
	 
	DynamicMesh* waves;
	MaterialOcean* waveMat;

	GLWidget* qtwidget;
	Image* heightImg;
	Image* normImg;

	DynamicMesh* fullscreenQuad;
	OceanGeometryGeneratorShader* oceanGeometryShader;

	// some tweakables
	bool drawWireframe;
	bool drawTextures;
	float lambda;

	OceanScene( SceneManager* manager );

	void PrepareGeometryRendering();

	bool drawSky;
	float refractionIndex;
	float buddha_angle;

	void init();
	void deinit();
	void update(float dt);
	void draw();
	void generateWaveGeometry();

};

#endif
