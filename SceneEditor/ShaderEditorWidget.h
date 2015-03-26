

#ifndef QT_SHADER_EDITOR_WIDGET_H
#define QT_SHADER_EDITOR_WIDGET_H
#include <GL/glew.h>
#include <qslider.h>
#include <qlineedit.h>
#include <map>
#include <qcolordialog.h>
#include <qtabwidget.h>
#include <QWidget>
#include <QListWidget>
#include <qtextedit.h>
#include <Shader.h>
using namespace std;

#include <boost/bind.hpp>
#include <boost/bimap.hpp>
using namespace boost;
using namespace boost::multi_index;

class GLWidget;
class QTweakable;
class QHBoxLayout;
class QVBoxLayout;
class Mesh;
class MyShader;

//**********************************
// ShaderEditorWidget : compile shaders and view the result in real time
//
//**********************************
class ShaderEditorWidget : public QWidget
{
    Q_OBJECT

public slots: 
	void compileShader();

signals:
	
public:
	GLWidget* glWidget;
	QHBoxLayout *mainLayout;
	QVBoxLayout* tweakableLayout;
	QTextEdit* VertexShaderEdit;
	QTextEdit* FragmentShaderEdit;

	MyShader* customShader;
public:
    ShaderEditorWidget();
    ~ShaderEditorWidget();

};

class MyShader : public Shader
{
public:
	MyShader();
	bool compile(const std::string& vertexSource, const std::string& fragmentSource);

	void Draw(Mesh* mesh);
};

#endif // QT_SHADER_EDITOR_WIDGET_H
