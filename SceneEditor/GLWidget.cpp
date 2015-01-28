#include "GLWidget.h"

#include <qwidget.h>
#include <qopengl.h>

#include <iostream>
#include <qevent.h>
#include "Camera.h"
#include <math.h>
#include "AntigravityConfig.h"
#include "SceneManager.h"

#ifndef M_PI
const double M_PI = 3.14159265359;
#endif

#ifndef M_2_PI
const double M_2_PI = M_PI / 2;
#endif

using std::cout;
using std::endl;

GLWidget::GLWidget(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent)
{
	setFocusPolicy(Qt::StrongFocus);

	sceneManager = new SceneManager();


	timer.setInterval(10);
	connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	timer.start();

}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(800, 600);
}

void GLWidget::setScene( const std::string& sceneName )
{
	sceneManager->setScene( sceneName );
}

void GLWidget::initializeGL()
{
	sceneManager->init();
	cout<<"Init done"<<endl;
}


void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sceneManager->update( 0.01f );
	sceneManager->draw();	
}

void GLWidget::resizeGL(int width, int height)
{
	sceneManager->resizeGL( width, height );
	glViewport(0,0,width,height);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int x = event->x();
	int y = event->y();
	bool leftButton   = event->buttons() & Qt::LeftButton;
	bool rightButton  = event->buttons() & Qt::RightButton;
	bool middleButton = event->buttons() & Qt::MiddleButton;
	sceneManager->mouseMoveEvent(x,y);
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
	 int numDegrees = event->delta() / 8;
     int numSteps = numDegrees / 15;

	 sceneManager->mouseWheelEvent( numSteps );
}

void GLWidget::keyPressEvent ( QKeyEvent * event )
{
	if( event->key() == Qt::Key::Key_Control )
	{
		sceneManager->keypress[MY_KEY::CONTROL] = true;
	}
}

void GLWidget::keyReleaseEvent ( QKeyEvent * event )
{
	if( event->key() == Qt::Key::Key_Control )
	{
		sceneManager->keypress[MY_KEY::CONTROL] = false;
	}
}