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
#include "AntigravityConfig.h"

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
	uniformChoice->addItem( QString("light pos (vec4)"));
	layout->addWidget(uniformName);
	layout->addWidget(uniformChoice);

	connect( uniformChoice,  SIGNAL( currentIndexChanged(int) ), this, SLOT( uniformChoiceChanged(int) ) );

	openImageButton = new QPushButton("open");
	image = new QLabel();
	image->setMaximumSize(QSize(100,100));
	image->setScaledContents(true);

	connect( openImageButton,  SIGNAL( clicked() ), this, SLOT( chooseImage() ) );
	layout->addWidget( openImageButton );
	layout->addWidget( image );
	openImageButton->setVisible(false);
	image->setVisible(false);

	slider = new QSlider( Qt::Orientation::Horizontal );
	slider->setVisible(false);
	slider->setRange(0,360);
	layout->addWidget(slider);

	layout->addStretch();

	QPushButton* minusButton = new QPushButton("-" );
	minusButton->setFixedSize( QSize( 40,20) );
	layout->addWidget( minusButton, Qt::AlignRight );
	connect( minusButton, &QPushButton::clicked, this, [this,parent](){ parent->removeWidget(this); } );

}

void UniformWidget::uniformChoiceChanged(int value)
{ 
	bool imageType = value >= UNIFORM_TYPE::TEXTURE2D_0 && value <= UNIFORM_TYPE::TEXTURE2D_3;
	openImageButton->setVisible(imageType);
	image->setVisible(imageType);
	
	bool lightPosType = value == UNIFORM_TYPE::LIGHT_POSITION;
	slider->setVisible(lightPosType);
	connect( slider, SIGNAL( valueChanged(int) ), this, SLOT( changeLightPos(int) ) );

}

void UniformWidget::changeLightPos(int value)
{
	float distance = 10.0f;
	float height = 5.0f;
	float angle = ((float)value) / 180.0f * 3.1415;
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4( cos(angle) * distance, height, sin(angle)* distance,1.0);

}

void UniformWidget::setImage(const std::string& filename )
{
	image->setPixmap( QPixmap( QString::fromStdString(filename) ) );

	parent->setUniformTexture( (UNIFORM_TYPE)uniformChoice->currentIndex(), QString::fromStdString(filename) );
}

void UniformWidget::chooseImage()
{
	QString file = QFileDialog::getOpenFileName(0,"","",QString("*.tga *.png"));
	
	if( !file.isEmpty() )
	{
		setImage( file.toStdString() );
	}
}
