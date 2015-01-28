#ifndef BUDDHA_SCENE_H
#define BUDDHA_SCENE_H
#include "GL\glew.h"
#include <QtCore/QCoreApplication>
#include <qwidget.h>

#include "Scene.h"


class OpenGLStaticModelInstance;
class MaterialPhongWithDepth;
class QITweakable;
class Image;
class DynamicMesh;
class TextureMixShader;
class SceneManager;
class QOpenGLFramebufferObject;
class FramebufferObject;

class BuddhaScene : public Scene
{
	Q_OBJECT

public:

	OpenGLStaticModelInstance*  buddha;
	MaterialPhongWithDepth* buddhaMat;
	
	Image* colorImage;
	Image* colorImage2;
	Image* colorImage3;

	DynamicMesh* fullscreenQuad;
	TextureMixShader* diffShader;

	QOpenGLFramebufferObject* fboqt;
	QOpenGLFramebufferObject* fboqt2;
	QOpenGLFramebufferObject* fboqt3;
	FramebufferObject* myFbo;
	FramebufferObject* myFbo2;

	GLuint myFboTex;
	GLuint myFboTex2;

	BuddhaScene(SceneManager* manager);

	bool initialized;

	float buddha_angle;
	float camera_distance;

	void init();
	void deinit();
	void update(float dt);
	void draw();
	void drawBuddha( bool depth );
	void computeDepthDifference();


};

#endif
