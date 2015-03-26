#include "mainwindow.h"

#include <iostream>
#include <string>
using namespace std;

#include <qpushbutton.h>
#include <qslider.h>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qvalidator.h>
#include <qlineedit.h>
#include <qscrollarea.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <boost/bind.hpp>
#include <QDoubleSpinBox>
#include <qsplitter.h>

#include "SceneManager.h"
#include "GLWidget.h"
#include "QTweakable.h"
#include "Scene.h"
#include "CustomQColorWidget.h"
#include "SceneEditorWidget.h"
#include "ShaderEditorWidget.h"
//**********************************
// Main Window
//
//**********************************
mainwindow::mainwindow( ) : QWidget()
{
	mainLayout = new QHBoxLayout;
	
	
	shaderEditorWidget = new ShaderEditorWidget();
	sceneEditorWidget  = new SceneEditorWidget();

	mainLayout->addWidget(shaderEditorWidget);
	//mainLayout->replaceWidget
	this->setLayout( mainLayout );
	this->setWindowTitle( "Scene Viewer" );
}

mainwindow::~mainwindow()
{
}