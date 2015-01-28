#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#define QT_NO_OPENGL_ES_2
#include <qwidget.h>
#include <qopengl.h>
#include <QtOpenGL\qgl.h>
#include <qtimer.h>

class SceneManager;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(const QGLFormat& format, QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

	bool sceneReady;
    QTimer timer;

	SceneManager* sceneManager;
	void setScene( const std::string& sceneName );


protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent* event);
	void keyPressEvent ( QKeyEvent * event );
	void keyReleaseEvent ( QKeyEvent * event );

	
};

#endif