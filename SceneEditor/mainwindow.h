

#ifndef QT_MAIN_WINDOW_H
#define QT_MAIN_WINDOW_H

#include <QMainWindow>
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

namespace Ui {
class mainwindow;
}

class GLWidget;
class QTweakable;
class QHBoxLayout;
class QVBoxLayout;

//**********************************
// Main Window
//
//**********************************
class mainwindow : public QWidget
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
    mainwindow();
    ~mainwindow();

};


#endif // QT_MAIN_WINDOW_H
