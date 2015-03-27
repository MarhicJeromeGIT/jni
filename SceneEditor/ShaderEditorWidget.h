

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
using namespace std;
#include <qlineedit.h>
#include <qcombobox.h>
#include <boost/bind.hpp>
#include <boost/bimap.hpp>
using namespace boost;
using namespace boost::multi_index;
#include <ShaderEditorScene.h>

class GLWidget;
class QTweakable;
class QHBoxLayout;
class QVBoxLayout;
class Mesh;

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
	QVBoxLayout* uniformsZone;
public:
    ShaderEditorWidget();
    ~ShaderEditorWidget();

private:
	ShaderEditorScene* shaderScene;
};

struct UniformWidget : public QWidget
{
	UniformWidget();
	QLineEdit* uniformName;
	QComboBox* uniformChoice;
};

struct AttributeWidget : public QWidget
{
	AttributeWidget();
	QLineEdit* attributeName;
	QComboBox* attributeChoice;
};

#endif // QT_SHADER_EDITOR_WIDGET_H
