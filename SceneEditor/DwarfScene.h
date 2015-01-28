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

class DwarfScene : public Scene
{
	Q_OBJECT

public:

	OpenGLStaticModelInstance* dwarfAnim;

	DwarfScene(SceneManager* manager);

	float dwarf_angle;

	void init();
	void deinit();
	void update(float dt);
	void draw();

};

#endif
