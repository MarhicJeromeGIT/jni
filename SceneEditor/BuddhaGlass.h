#ifndef BUDDHA_GLASS_H
#define BUDDHA_GLASS_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>

#include "Scene.h"


class OpenGLStaticModelInstance;
class MaterialGlass;
class GLWidget;
class QITweakable;
class Skybox;
class SceneManager;

class BuddhaGlass : public Scene
{
	Q_OBJECT

public:
	Skybox* skybox;

	OpenGLStaticModelInstance*  buddha; 
	MaterialGlass* buddhaMat;
	GLWidget* qtwidget;

	BuddhaGlass( SceneManager* manager );

	bool drawSky;
	float refractionIndex;
	float buddha_angle;

	void init();
	void deinit();
	void update(float dt);
	void draw();

};

#endif
