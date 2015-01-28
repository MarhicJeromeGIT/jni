#ifndef DWARF_SCENE_H
#define DWARF_SCENE_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>

#include "Scene.h"


class OpenGLStaticModelInstance;
class MaterialGlass;
class QITweakable;
class Skybox;
class OpenGLSkinnedModelInstance;
class Image;
class MaterialBump;
class Skybox;
class DwarfScene : public Scene
{
	Q_OBJECT

public:
	Skybox* skybox;

	OpenGLStaticModelInstance* dwarfAnim;
	OpenGLStaticModelInstance* castleInstance;

	DwarfScene(SceneManager* manager);

	float dwarf_angle;
	bool drawTextures;

	MaterialBump* armorMaterial;

	void init();
	void deinit();
	void update(float dt);
	void draw();

	Image* armorTexture;

};

#endif
