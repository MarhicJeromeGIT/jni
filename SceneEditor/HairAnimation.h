#ifndef HAIR_ANIMATION_H
#define HAIR_ANIMATION_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>

#include "Scene.h"


class OpenGLStaticModelInstance;
class MaterialGlass;
class GLWidget;
class QITweakable;
class Skybox;
class SceneManager;

class HairAnimation : public Scene
{
	Q_OBJECT

public:
	Skybox* skybox;

	HairAnimation( SceneManager* manager );

	bool drawSky;

	void init();
	void deinit();
	void update(float dt);
	void draw();
};

#endif
