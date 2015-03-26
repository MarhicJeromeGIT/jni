#include "ShaderEditorWidget.h"
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

//**********************************
// ShaderEditorWidget 
//
//**********************************
ShaderEditorWidget::ShaderEditorWidget( ) : QWidget()
{
	mainLayout = new QHBoxLayout;
	
	QSplitter* splitter = new QSplitter(this);
	mainLayout->addWidget( splitter );

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

	glWidget->setScene("ShaderEditorScene");

	VertexShaderEdit = new QTextEdit();
	FragmentShaderEdit = new QTextEdit();

	QHBoxLayout* buttonZone = new QHBoxLayout();
	QPushButton* compileButon = new QPushButton("Compile");
	buttonZone->addWidget(compileButon);
	QPushButton* saveButton = new QPushButton("Save");
	buttonZone->addWidget(saveButton);

	tweakableLayout->addLayout(buttonZone);
	tweakableLayout->addWidget(VertexShaderEdit);
	tweakableLayout->addWidget(FragmentShaderEdit);


	connect( compileButon, SIGNAL( clicked() ), this, SLOT( compileShader() ) );

	customShader = new MyShader();

}

ShaderEditorWidget::~ShaderEditorWidget()
{
}

void ShaderEditorWidget::compileShader()
{
	cout<<"compiling shader"<<endl;
	cout<<VertexShaderEdit->toPlainText().toStdString()<<endl;

	customShader->compile( VertexShaderEdit->toPlainText().toStdString(),
						   FragmentShaderEdit->toPlainText().toStdString());
}

MyShader::MyShader()
{
}

// return true if no compilation error
bool MyShader::compile(const std::string& vertexSource, const std::string& fragmentSource)
{
	newCompileProgram( vertexSource, fragmentSource );

	return true;
}

void MyShader::Draw(Mesh* mesh)
{
}
