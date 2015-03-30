

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
struct UniformWidget;
struct AttributeWidget;


//**********************************
// ShaderEditorWidget : compile shaders and view the result in real time
//
//**********************************
class ShaderEditorWidget : public QWidget
{
    Q_OBJECT

public slots: 
	void compileShader();
	void setDefaults();
	void saveCurrentState();
	void openCurrentState();
	void clearContent();

signals:
	
public:
	GLWidget* glWidget;
	QVBoxLayout *mainLayout;
	QVBoxLayout* tweakableLayout;
	QTextEdit* VertexShaderEdit;
	QTextEdit* FragmentShaderEdit;
	QVBoxLayout* uniformsZone;
	QVBoxLayout* attributesZone;
public:
    ShaderEditorWidget();
    ~ShaderEditorWidget();

	void BuildUniformMap(map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap);
	void BuildAttributeMap(map<std::string,ATTRIBUTE_TYPE>& aliasToAttributeTypeMap);

	void removeWidget( UniformWidget* widget );

	void setUniformTexture( UNIFORM_TYPE type, const QString& filename );

private:
	ShaderEditorScene* shaderScene;
	QString texture0Filename;

	vector<UniformWidget*>   addedUniformWidgets;
	vector<AttributeWidget*> addedAttributeWidgets;
};

struct AttributeWidget : public QWidget
{
	AttributeWidget();
	QLineEdit* attributeName;
	QComboBox* attributeChoice;
};

#endif // QT_SHADER_EDITOR_WIDGET_H
