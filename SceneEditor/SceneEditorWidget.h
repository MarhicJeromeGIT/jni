

#ifndef QT_SCENE_EDITOR_WIDGET_H
#define QT_SCENE_EDITOR_WIDGET_H

#include <qslider.h>
#include <qlineedit.h>
#include <map>
#include <qcolordialog.h>
#include <qtabwidget.h>
#include <QWidget>
#include <QListWidget>

using namespace std;

#include <boost/bind.hpp>
#include <boost/bimap.hpp>
using namespace boost;
using namespace boost::multi_index;

class GLWidget;
class QTweakable;
class QHBoxLayout;
class QVBoxLayout;

//**********************************
// Main Window
//
//**********************************
class SceneEditorWidget : public QWidget
{
    Q_OBJECT

public slots: 
     void tweakableAdded(QTweakable*);
	 void currentRowChanged ( int index );

signals:
	//void signalTweaksReceived();

public:
	GLWidget* glWidget;
	QHBoxLayout *mainLayout;
	QVBoxLayout* tweakableLayout;

	QListWidget* listWidget;
public:
    SceneEditorWidget();
    ~SceneEditorWidget();

};


#endif // QT_SCENE_EDITOR_WIDGET_H
