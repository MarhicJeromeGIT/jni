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
#include "qaction.h"

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
//	shaderScene = make_shared<ShaderEditorScene>( glWidget->getScene("ShaderEditorScene") );
	shaderScene = (ShaderEditorScene*) glWidget->getScene("ShaderEditorScene");
	assert( shaderScene != nullptr );

	VertexShaderEdit = new QTextEdit();
	FragmentShaderEdit = new QTextEdit();

	QHBoxLayout* buttonZone = new QHBoxLayout();
	QPushButton* compileButon = new QPushButton("Compile");
	buttonZone->addWidget(compileButon);
	QPushButton* saveButton = new QPushButton("Save");
	buttonZone->addWidget(saveButton);

	uniformsZone = new QVBoxLayout();
	QPushButton* addUniformButton = new QPushButton("+");
	UniformWidget* unif1 = new UniformWidget();
	uniformsZone->addWidget(addUniformButton);
	uniformsZone->addWidget(unif1);

	connect( addUniformButton,  &QPushButton::clicked, this, [this]() { 
		UniformWidget* newUnif = new UniformWidget();
		uniformsZone->addWidget(newUnif);  
	} );

	QVBoxLayout* attributesZone = new QVBoxLayout();
	QPushButton* addAttributeButton = new QPushButton("+");
	AttributeWidget* attrib1 = new AttributeWidget();
	attributesZone->addWidget(addAttributeButton);
	attributesZone->addWidget(attrib1);
	connect( addAttributeButton,  &QPushButton::clicked, this, [attributesZone]() { 
		AttributeWidget* newAttrib = new AttributeWidget();
		attributesZone->addWidget(newAttrib);  
	} );

	

	tweakableLayout->addLayout(buttonZone);
	tweakableLayout->addLayout(uniformsZone);
	tweakableLayout->addLayout(attributesZone);
	tweakableLayout->addWidget(VertexShaderEdit);
	tweakableLayout->addWidget(FragmentShaderEdit);

	string vs = 
"uniform mat4 ModelMatrix; \n\
uniform mat4 ViewMatrix; \n\
uniform mat4 ProjectionMatrix; \n\
in vec3 vertexPosition; \n\
void main(void) \n\
{	 \n\
	gl_Position  = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0); \n\
}\n";
	VertexShaderEdit->setText( QString::fromStdString( vs ) );
	string ps = 
"void main(void)  \n\
{  \n\
	gl_FragColor = vec4(0.3,0.3,0.3,1.0); \n\
} \n";
	FragmentShaderEdit->setText( QString::fromStdString( ps ) );

	connect( compileButon, SIGNAL( clicked() ), this, SLOT( compileShader() ) );

}

ShaderEditorWidget::~ShaderEditorWidget()
{
}

void ShaderEditorWidget::compileShader()
{
	cout<<"compiling shader"<<endl;
	cout<<VertexShaderEdit->toPlainText().toStdString()<<endl;

	// Build the uniform bindings map :
	map<std::string,UNIFORM_TYPE> aliasToUniformTypeMap;
	QObjectList widgetList = uniformsZone->children();
	for( int id = 0; id < uniformsZone->count(); id++ )
	{
		// QWidgetItem
		QLayoutItem* thisObject = uniformsZone->itemAt(id);
		if( dynamic_cast<QWidgetItem*>(thisObject) != 0 )
		{
			UniformWidget* myWidget = 0;
			if( (myWidget = dynamic_cast<UniformWidget*>( thisObject->widget() ) ) != 0  )
			{
				UNIFORM_TYPE cast[] = { MODEL_MATRIX,	VIEW_MATRIX, PROJECTION_MATRIX,	NORMAL_MATRIX }; // annoying
				aliasToUniformTypeMap.insert( pair<std::string,UNIFORM_TYPE>( myWidget->uniformName->text().toStdString(), cast[ myWidget->uniformChoice->currentIndex() ] ) );		
			}
		}
	}
	
	shaderScene->customMat->compile( VertexShaderEdit->toPlainText().toStdString(),
									 FragmentShaderEdit->toPlainText().toStdString(),
									 aliasToUniformTypeMap );
}



UniformWidget::UniformWidget() : QWidget()
{
	QHBoxLayout* layout = new QHBoxLayout();
	setLayout(layout);

	uniformName = new QLineEdit();
	uniformChoice = new QComboBox();
	uniformChoice->addItem( QString("Model Matrix (mat4)") );
	uniformChoice->addItem( QString("View Matrix (mat4)") );
	uniformChoice->addItem( QString("Projection Matrix (mat4)") );
	uniformChoice->addItem( QString("Normal Matrix (mat3)") );

	layout->addWidget(uniformName);
	layout->addWidget(uniformChoice);
}

AttributeWidget::AttributeWidget() : QWidget()
{
	QHBoxLayout* layout = new QHBoxLayout();
	setLayout(layout);

	attributeName = new QLineEdit();
	attributeChoice = new QComboBox();
	attributeChoice->addItem( QString("vertex position (vec3)") );

	layout->addWidget(attributeName);
	layout->addWidget(attributeChoice);
}
	