#ifndef SCENE_H
#define SCENE_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>

class QTweakable;
class SceneManager;

class Scene : public QObject
{
	Q_OBJECT

Q_SIGNALS:
	void tweakableAdded( QTweakable* );

public:
	SceneManager* sceneManager;

public:
	Scene( SceneManager* sceneManager );

	virtual void init() = 0;
	virtual void deinit() = 0;

	virtual void update( float dt ) = 0;
	virtual void draw() = 0;

	virtual void mouseMoveEvent( int x, int y ){}
	virtual void mouseWheelEvent( int numSteps ){}
};

#endif