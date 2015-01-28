#include "QApplication"
#include "QGLWidget"
#include "QDebug"
#include "cmath"

#define M_PI 3.14156

class GLWidget : public QGLWidget{
    void initializeGL(){
        /// In modelview hand is at origin   
        glClearColor(1.0, 1.0, 1.0, 1.0); ///< essential using white, as labels start from 0!!!!!         glEnable(GL_DEPTH_TEST);         glDisable(GL_LIGHTING);         glDisable(GL_DOUBLEBUFFER);         glEnableClientState(GL_VERTEX_ARRAY);                  /// Output opengl version         qDebug("OpenGL %d.%d",this->format().majorVersion(),this->format().minorVersion());
    }
    
    void qgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar){
        const GLdouble ymax = zNear * tan(fovy * M_PI / 360.0);
        const GLdouble ymin = -ymax;
        const GLdouble xmin = ymin * aspect;
        const GLdouble xmax = ymax * aspect;
        glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
    }
    
    /// @note camera decides renderer size
    void resizeGL(int width, int height){
    	if (height==0) height=1;
    	glViewport(0,0,width,height);
    	glMatrixMode(GL_PROJECTION);
    	glLoadIdentity();
    	qgluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
    	glMatrixMode(GL_MODELVIEW);
    	glLoadIdentity();
     }
    
    void paintGL(){
        glMatrixMode(GL_MODELVIEW);         
        glLoadIdentity();
        glClear(GL_COLOR_BUFFER_BIT);  

        glBegin(GL_POLYGON); 
            glVertex2f(-0.5, -0.5); 
            glVertex2f(-0.5, 0.5);
            glVertex2f(0.5, 0.5); 
            glVertex2f(0.5, -0.5); 
        glEnd();
    }
};

int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    GLWidget widget;
    widget.resize(640,480);
    widget.show();
    return app.exec();
}