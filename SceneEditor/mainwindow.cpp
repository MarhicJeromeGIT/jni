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

//**********************************
// Main Window
//
//**********************************
mainwindow::mainwindow( ) : QWidget()
{
	mainLayout = new QHBoxLayout;
	
	QSplitter* splitter = new QSplitter(this);
	mainLayout->addWidget( splitter );

	listWidget = new QListWidget;
	splitter->addWidget(listWidget);

	QGLFormat format;
	cout<<format.openGLVersionFlags();

	format.setVersion(2,1);
	format.setProfile( QGLFormat::CompatibilityProfile );
	format.setDoubleBuffer(true);
	format.setSampleBuffers( true );

	glWidget = new GLWidget(format);
	splitter->addWidget(glWidget);

	tweakableLayout = new QVBoxLayout;
	tweakableLayout->setAlignment( Qt::AlignTop );
	QWidget* rightSide = new QWidget();
	rightSide->setLayout( tweakableLayout );

	QScrollArea *frame_scroll = new QScrollArea();  
	frame_scroll->setWidgetResizable(true);
	frame_scroll->setWidget( rightSide );

	splitter->addWidget( frame_scroll );

	this->setLayout( mainLayout );
	this->setWindowTitle( "Scene Viewer" );

	for( int i=0; i< glWidget->sceneManager->scenesItemArray.size(); i++ )
	{
		connect( glWidget->sceneManager->scenesItemArray[i].second, SIGNAL( tweakableAdded( QTweakable* ) ), this, SLOT( tweakableAdded( QTweakable* ) ) );
	}
	
	pair<string,Scene*> SceneItem;

	QListWidgetItem* item = new QListWidgetItem(tr("Glass Refraction/Reflection"), listWidget);
	//listWidget->setStyleSheet( "QListWidget::item { border-bottom: 1px solid black; padding-left: 5px; }" );

	new QListWidgetItem(tr("Grass Scene"), listWidget );
	new QListWidgetItem(tr("Dual Depth Peeling"), listWidget);
	new QListWidgetItem(tr("Textured Dwarf"), listWidget);
	new QListWidgetItem(tr("Ocean"), listWidget);

	QListWidgetItem* separator = new QListWidgetItem( "Work In Progress" );
	separator->setSizeHint(QSize(10,30));
	separator->setFlags( Qt::NoItemFlags );
	listWidget->addItem(separator);

	new QListWidgetItem(tr("Skeletal Animation"), listWidget );

	listWidget->addItem(separator);
	new QListWidgetItem(tr("Godrays"), listWidget );
	new QListWidgetItem(tr("Instancing"), listWidget );
	new QListWidgetItem(tr("Tesselation"), listWidget );
	new QListWidgetItem(tr("Ray Tracing"), listWidget );
	new QListWidgetItem(tr("Movement Blur"), listWidget );

	// fire, water, magic etc etc !
	connect( listWidget, SIGNAL( currentRowChanged ( int ) ), this, SLOT( currentRowChanged( int ) ) );
}

mainwindow::~mainwindow()
{
}

void mainwindow::tweakableAdded(QTweakable* utweak )
{
	switch( utweak->type )
	{
		case TWEAKER_TYPE::TWEAKER_INTEGER :
		{
			QIntTweakable* itweak = dynamic_cast<QIntTweakable*>( utweak );

			QLabel* label = new QLabel( QString( itweak->name.c_str() ) );
			QSlider* slider = new QSlider(Qt::Horizontal, this );
			slider->setRange( itweak->iMin, itweak->iMax);
			slider->setValue( itweak->iValue );

			tweakableLayout->addWidget( label );
			tweakableLayout->addWidget( slider );

			connect( slider, SIGNAL(valueChanged(int)), itweak, SLOT(doCallback(int)) );
			connect( slider, &QSlider::valueChanged, [label, itweak](int p){ label->setText( QString(itweak->name.c_str() + QString(":") + QString::number(p) ) ); } );
		}
		break;

		case TWEAKER_TYPE::TWEAKER_COLOR:
		{
			QColorTweakable* ctweak = dynamic_cast<QColorTweakable*>(utweak);

			QLabel* label = new QLabel( QString( ctweak->name.c_str() ) );
		//	QColorDialog* colorPicker = new QColorDialog( this );
			CustomQColorWidget* colorPicker = new CustomQColorWidget( this );
			 
			tweakableLayout->addWidget( label );
			tweakableLayout->addWidget( colorPicker );

			connect( colorPicker, SIGNAL(colorChanged(const QColor &)), ctweak, SLOT(doCallback(const QColor &)) );
		}
		break;

		case TWEAKER_TYPE::TWEAKER_BOOL:
		{
			QBooleanTweakable* btweak = dynamic_cast<QBooleanTweakable*>(utweak);

			QCheckBox* checkbox = new QCheckBox(btweak->name.c_str() );
			checkbox->setChecked( btweak->bValue );

			tweakableLayout->addWidget( checkbox );

			connect( checkbox, SIGNAL( stateChanged(int) ), btweak, SLOT(doCallback( int )) );
		}
		break;

		case TWEAKER_TYPE::TWEAKER_FLOAT:
		{
			QFloatTweakable* ftweak = dynamic_cast<QFloatTweakable*>(utweak);

			QLabel* label = new QLabel( QString( ftweak->name.c_str() ) );
			QDoubleSpinBox* box = new QDoubleSpinBox();
			tweakableLayout->addWidget( label );
			tweakableLayout->addWidget( box );

			box->setValue( ftweak->fValue );
			box->setRange( ftweak->fMin, ftweak->fMax );
			box->setSingleStep(0.01);
			connect( box, SIGNAL( valueChanged(double) ), ftweak, SLOT( doCallback(double) ) );
		}
		break;
	}
}

void clearLayout(QLayout* layout, bool deleteWidgets = true)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                delete widget;
        }
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }

}
void mainwindow::currentRowChanged( int index )
{
	QListWidgetItem* item = listWidget->item(index);
	if( item != NULL )
	{
		QString title = item->text();
		if(!title.isEmpty() )
		{
			if(tweakableLayout != NULL && !tweakableLayout->isEmpty() )
			{
				clearLayout( tweakableLayout );
			}

			glWidget->setScene(title.toStdString());
		}
	}
}