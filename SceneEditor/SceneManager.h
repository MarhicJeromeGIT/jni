#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>
#include <vector>

class QTweakable;
class Camera;
class Scene;

enum MY_KEY
{
	CONTROL,
	MAX_KEYS,
};

class SceneManager
{
public:
	SceneManager();

	void init();
	void deinit();

	void update( float dt );
	void draw();

	void resizeGL( int width, int height );

	void mouseMoveEvent( int x, int y );
	void mouseWheelEvent( int numSteps );

public:
	Camera* camera;

	float camera_distance;

	void setScene( const std::string& sceneName );

	std::vector< std::pair<std::string,Scene*> > scenesItemArray;

	int currentScene;
	bool ready;

	bool keypress[MY_KEY::MAX_KEYS];

};

#endif