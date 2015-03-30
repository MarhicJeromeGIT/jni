#include "UniformWidget.h"
#include <GL/glew.h>
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
#include "qaction.h"
#include "TextureGL.h"
#include "qfiledialog.h"

#include "tinyxml.h"
#include "ShaderEditorWidget.h"

UniformWidget::UniformWidget(ShaderEditorWidget* parent) : QWidget(), parent(parent)
{
	layout = new QHBoxLayout();
	setLayout(layout);

	uniformName = new QLineEdit();
	uniformChoice = new QComboBox();
	uniformChoice->addItem( QString("Model Matrix (mat4)") );
	uniformChoice->addItem( QString("View Matrix (mat4)") );
	uniformChoice->addItem( QString("Projection Matrix (mat4)") );
	uniformChoice->addItem( QString("Normal Matrix (mat3)") );
	uniformChoice->addItem( QString("Inv View Mat (mat4)") );
	uniformChoice->addItem( QString("Time (float)") );
	uniformChoice->addItem( QString("tex0 (Sampler2D)") );
	uniformChoice->addItem( QString("tex1 (Sampler2D)") );
	uniformChoice->addItem( QString("tex2 (Sampler2D)") );
	uniformChoice->addItem( QString("tex3 (Sampler2D)") );
	uniformChoice->addItem( QString("cubemap (samplerCube)") );

	layout->addWidget(uniformName);
	layout->addWidget(uniformChoice);

	QPushButton* minusButton = new QPushButton("-");
	layout->addWidget( minusButton );
	connect( minusButton, &QPushButton::clicked, this, [this,parent](){ parent->removeWidget(this); } );
	connect( uniformChoice,  SIGNAL( currentIndexChanged(int) ), this, SLOT( uniformChoiceChanged(int) ) );

	openImageButton = nullptr;
	image = nullptr;
}

void UniformWidget::uniformChoiceChanged(int value)
{ 
	if( value >= UNIFORM_TYPE::TEXTURE2D_0 && value <= UNIFORM_TYPE::TEXTURE2D_3 )
	{
		openImageButton = new QPushButton("open");
		connect( openImageButton,  SIGNAL( clicked() ), this, SLOT( chooseImage() ) );
		image = new QLabel();

		layout->addWidget( openImageButton );
		layout->addWidget( image );
	}
	else
	{
		if( openImageButton != nullptr && image != nullptr )
		{
			layout->removeWidget( openImageButton );
			layout->removeWidget( image );

			delete openImageButton;
			delete image;
		}
	}
}

void UniformWidget::chooseImage()
{
	QString file = QFileDialog::getOpenFileName(0,"","",QString("*.tga *.png"));
	
	if( !file.isEmpty() )
	{
		 image->setPixmap( QPixmap( file ) );
	}
	
	parent->setUniformTexture( (UNIFORM_TYPE)uniformChoice->currentIndex(), file );
}
