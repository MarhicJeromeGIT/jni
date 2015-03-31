

#ifndef QT_UNIFORM_WIDGET_H
#define QT_UNIFORM_WIDGET_H
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
class QLabel;
class QPUshButton;

class ShaderEditorWidget;

struct UniformWidget : public QWidget
{
	Q_OBJECT

public slots:
	void uniformChoiceChanged(int value);
	void chooseImage();
	void setImage(const std::string& filename );
	void changeLightPos(int value);
public:
	ShaderEditorWidget* parent;

	QHBoxLayout* layout;
	UniformWidget( ShaderEditorWidget* parent );
	QLineEdit* uniformName;
	QComboBox* uniformChoice;

	QPushButton* openImageButton;
	QLabel *image;

	QSlider* slider;

};

#endif
