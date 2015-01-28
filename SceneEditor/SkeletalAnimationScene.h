#ifndef SKELETAL_ANIMATION_SCENE_H
#define SKELETAL_ANIMATION_SCENE_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>

#include "Scene.h"


class OpenGLStaticModelInstance;
class MaterialGlass;
class QITweakable;
class Skybox;
class OpenGLSkinnedModelInstance;

class SkeletalAnimationScene : public Scene
{
	Q_OBJECT

public:
	Skybox* skybox;
	OpenGLSkinnedModelInstance* animatedModel;

	SkeletalAnimationScene(SceneManager* manager);

	float dwarf_angle;

	void init();
	void deinit();
	void update(float dt);
	void draw();

};

#endif
