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
#include "TextureGL.h"
#include "qfiledialog.h"

#include "tinyxml.h"
//**********************************
// ShaderEditorWidget 
//
//**********************************
ShaderEditorWidget::ShaderEditorWidget( ) : QWidget()
{
	mainLayout = new QVBoxLayout;
	
	QSplitter* splitter = new QSplitter(this);

	QGLFormat format;
	cout<<format.openGLVersionFlags();

	format.setVersion(2,1);
	format.setProfile( QGLFormat::CompatibilityProfile );
	format.setDoubleBuffer(true);
	format.setSampleBuffers( true );

	QHBoxLayout* topButtonsLayout = new QHBoxLayout();
	QPushButton* saveButton = new QPushButton( QString("Save") );
	QPushButton* openButton = new QPushButton( QString("Open") );
	QPushButton* sphereButton = new QPushButton( QString("Sphere") );
	QPushButton* planeButton = new QPushButton( QString("Plane") );
	topButtonsLayout->addWidget( saveButton );
	topButtonsLayout->addWidget( openButton );
	topButtonsLayout->addWidget( sphereButton );
	topButtonsLayout->addWidget( planeButton );
	connect( saveButton,   &QPushButton::clicked, this, [this](){ saveCurrentState(); } );
	connect( openButton,   &QPushButton::clicked, this, [this](){ openCurrentState(); } );
	connect( sphereButton, &QPushButton::clicked, this, [this](){ shaderScene->setMesh(EDITOR_MESH_TYPE::EDITOR_SPHERE_MESH); } );
	connect( planeButton,  &QPushButton::clicked, this, [this](){ shaderScene->setMesh(EDITOR_MESH_TYPE::EDITOR_PLANE_MESH); } );

	mainLayout->addLayout( topButtonsLayout );
	mainLayout->addWidget( splitter );

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
	QPushButton* loadTextureButton = new QPushButton("Load Texture");
	buttonZone->addWidget(loadTextureButton);

	uniformsZone = new QVBoxLayout();
	attributesZone = new QVBoxLayout();


	tweakableLayout->addLayout(buttonZone);
	tweakableLayout->addLayout(uniformsZone);
	tweakableLayout->addLayout(attributesZone);
	tweakableLayout->addWidget(VertexShaderEdit);
	tweakableLayout->addWidget(FragmentShaderEdit);

	connect( compileButon, SIGNAL( clicked() ), this, SLOT( compileShader() ) );
	connect( loadTextureButton, SIGNAL( clicked() ), this, SLOT( loadTexture() ) );

	setDefaults();
}

// clear everything (attributes, uniforms, shaders text)
void ShaderEditorWidget::clearContent()
{
	// remove the UniformWidget from the layout :
	for( int i = 0; i< addedUniformWidgets.size(); i++ )
	{
		uniformsZone->removeWidget( addedUniformWidgets[i] );
		delete addedUniformWidgets[i];
	}
	addedUniformWidgets.clear();

	// remove the AttributeWidget from the layout :
	for( int i = 0; i< addedAttributeWidgets.size(); i++ )
	{
		attributesZone->removeWidget( addedAttributeWidgets[i] );
		delete addedAttributeWidgets[i];
	}
	addedAttributeWidgets.clear();

	VertexShaderEdit->clear();
	FragmentShaderEdit->clear();
}

void ShaderEditorWidget::setDefaults()
{
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

	QPushButton* addUniformButton = new QPushButton("+");
	uniformsZone->addWidget(addUniformButton);
	UniformWidget* unif1 = new UniformWidget();
	uniformsZone->addWidget(unif1);
	addedUniformWidgets.push_back(unif1);
	unif1->uniformName->setText( QString("ModelMatrix") );
	unif1->uniformChoice->setCurrentIndex( (int) MODEL_MATRIX );
	UniformWidget* unif2 = new UniformWidget();
	uniformsZone->addWidget(unif2);
	addedUniformWidgets.push_back(unif2);
	unif2->uniformName->setText( QString("ViewMatrix") );
	unif2->uniformChoice->setCurrentIndex( (int) VIEW_MATRIX );
	UniformWidget* unif3 = new UniformWidget();
	uniformsZone->addWidget(unif3);
	addedUniformWidgets.push_back(unif3);
	unif3->uniformName->setText( QString("ProjectionMatrix") );
	unif3->uniformChoice->setCurrentIndex( (int) PROJECTION_MATRIX );

	connect( addUniformButton,  &QPushButton::clicked, this, [this]() { 
		UniformWidget* newUnif = new UniformWidget();
		addedUniformWidgets.push_back(newUnif);
		uniformsZone->addWidget(newUnif);  
	} );

	QPushButton* addAttributeButton = new QPushButton("+");
	attributesZone->addWidget(addAttributeButton);
	AttributeWidget* attrib1 = new AttributeWidget();
	attrib1->attributeName->setText( QString("vertexPosition") );
	attrib1->attributeChoice->setCurrentIndex( (int) ATTRIBUTE_TYPE::POSITION );
	attributesZone->addWidget(attrib1);
	addedAttributeWidgets.push_back(attrib1);
	connect( addAttributeButton,  &QPushButton::clicked, this, [this]() { 
		AttributeWidget* newAttrib = new AttributeWidget();
		addedAttributeWidgets.push_back(newAttrib);
		attributesZone->addWidget(newAttrib);  
	} );
}

ShaderEditorWidget::~ShaderEditorWidget()
{
}

void ShaderEditorWidget::BuildUniformMap(map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap)
{
	for( int id = 0; id < uniformsZone->count(); id++ )
	{
		// QWidgetItem
		QLayoutItem* thisObject = uniformsZone->itemAt(id);
		if( dynamic_cast<QWidgetItem*>(thisObject) != 0 )
		{
			UniformWidget* myWidget = 0;
			if( (myWidget = dynamic_cast<UniformWidget*>( thisObject->widget() ) ) != 0  )
			{
				aliasToUniformTypeMap.insert( pair<std::string,UNIFORM_TYPE>( myWidget->uniformName->text().toStdString(), (UNIFORM_TYPE)myWidget->uniformChoice->currentIndex() ) );		
			}
		}
	}
}

void ShaderEditorWidget::BuildAttributeMap(map<std::string,ATTRIBUTE_TYPE>& aliasToAttributeTypeMap)
{
	for( int id = 0; id < attributesZone->count(); id++ )
	{
		// QWidgetItem
		QLayoutItem* thisObject = attributesZone->itemAt(id);
		if( dynamic_cast<QWidgetItem*>(thisObject) != 0 )
		{
			AttributeWidget* myWidget = 0;
			if( (myWidget = dynamic_cast<AttributeWidget*>( thisObject->widget() ) ) != 0  )
			{
				aliasToAttributeTypeMap.insert( pair<std::string,ATTRIBUTE_TYPE>( myWidget->attributeName->text().toStdString(), (ATTRIBUTE_TYPE)myWidget->attributeChoice->currentIndex() ) );		
			}
		}
	}
}

void ShaderEditorWidget::compileShader()
{
	cout<<"compiling shader"<<endl;
	cout<<VertexShaderEdit->toPlainText().toStdString()<<endl;

	// Build the uniform bindings map :
	map<std::string,UNIFORM_TYPE> aliasToUniformTypeMap;
	BuildUniformMap(aliasToUniformTypeMap);

	// Build the attributes bindings map :
	map<std::string,ATTRIBUTE_TYPE> aliasToAttributeTypeMap;
	BuildAttributeMap(aliasToAttributeTypeMap);

	shaderScene->customMat->compile( VertexShaderEdit->toPlainText().toStdString(),
									 FragmentShaderEdit->toPlainText().toStdString(),
									 aliasToUniformTypeMap,
									 aliasToAttributeTypeMap,
									 shaderScene->customParams );
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
	uniformChoice->addItem( QString("Inv View Mat (mat4)") );
	uniformChoice->addItem( QString("Time (float)") );
	uniformChoice->addItem( QString("tex0 (Sampler2D)") );
	uniformChoice->addItem( QString("tex1 (Sampler2D)") );
	uniformChoice->addItem( QString("tex2 (Sampler2D)") );
	uniformChoice->addItem( QString("tex3 (Sampler2D)") );
	uniformChoice->addItem( QString("cubemap (samplerCube)") );

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
	attributeChoice->addItem( QString("texture coord (vec2)") );
	attributeChoice->addItem( QString("normal (vec3)") );
	attributeChoice->addItem( QString("tangent (vec3)") );

	layout->addWidget(attributeName);
	layout->addWidget(attributeChoice);
}


void ShaderEditorWidget::loadTexture()
{
	QString file = QFileDialog::getOpenFileName(0,"","",QString("*.tga *.png"));
	texture0Filename = file;

	TextureManager::get()->setTexturePath("");
	if( !texture0Filename.isEmpty() )
	{
		TextureGL* tex = TextureManager::get()->loadTexture(texture0Filename.toStdString(), texture0Filename.toStdString() );
		if( tex != NULL )
		{
			shaderScene->customParams.tex0 = tex;
		}
	}
}


void ShaderEditorWidget::saveCurrentState()
{
	QString filename = QFileDialog::getSaveFileName();
	TiXmlDocument newdoc(filename.toStdString() );

	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "utf-8", "");
	newdoc.LinkEndChild( decl );

	TiXmlElement* UniformsElement = new TiXmlElement( "uniforms" );
	newdoc.LinkEndChild( UniformsElement );

	// Build the uniform bindings map :
	map<std::string,UNIFORM_TYPE> aliasToUniformTypeMap;
	BuildUniformMap(aliasToUniformTypeMap);
	for( auto it = aliasToUniformTypeMap.begin(); it != aliasToUniformTypeMap.end(); it++ )
	{
		TiXmlElement* newUniform = new TiXmlElement( "uniform" );
		newUniform->SetAttribute("name", it->first );
		newUniform->SetAttribute("value", (int)it->second );
		UniformsElement->LinkEndChild( newUniform );
	}

	TiXmlElement* AttributesElement = new TiXmlElement( "attributes" );
	newdoc.LinkEndChild( AttributesElement );

	// Build the attributes bindings map :
	map<std::string,ATTRIBUTE_TYPE> aliasToAttributeTypeMap;
	BuildAttributeMap(aliasToAttributeTypeMap);
	for( auto it = aliasToAttributeTypeMap.begin(); it != aliasToAttributeTypeMap.end(); it++ )
	{
		TiXmlElement* newAttribute = new TiXmlElement( "attribute" );
		newAttribute->SetAttribute("name", it->first );
		newAttribute->SetAttribute("value", (int)it->second );
		AttributesElement->LinkEndChild( newAttribute );
	}

	TiXmlElement* VertexShaderElement = new TiXmlElement( "vertex_shader" );
	newdoc.LinkEndChild( VertexShaderElement );
	TiXmlText* vertexShaderContent = new TiXmlText(VertexShaderEdit->toPlainText().toStdString());
	VertexShaderElement->LinkEndChild(vertexShaderContent);

	TiXmlElement* FragmentShaderElement = new TiXmlElement( "fragment_shader" );
	newdoc.LinkEndChild( FragmentShaderElement );
	TiXmlText* fragmentShaderContent = new TiXmlText(FragmentShaderEdit->toPlainText().toStdString());
	FragmentShaderElement->LinkEndChild(fragmentShaderContent);
	
	newdoc.SaveFile();
}

void ShaderEditorWidget::openCurrentState()
{
	QString filename = QFileDialog::getOpenFileName();

	TiXmlDocument doc;
	if( !doc.LoadFile( filename.toStdString() ) )
	{
		return;
	}
	TiXmlHandle hdl(&doc);

	clearContent();

	TiXmlElement* UniformsElement = hdl.FirstChildElement( "uniforms" ).Element();
	if( UniformsElement != nullptr )
	{
		TiXmlElement* uniform = UniformsElement->FirstChildElement();
		while( uniform != nullptr )
		{
			int value = 0;
			uniform->QueryIntAttribute("value",&value);
			std::string name;
			uniform->QueryStringAttribute("name",&name);
	
			UniformWidget* unif1 = new UniformWidget();
			uniformsZone->addWidget(unif1);
			addedUniformWidgets.push_back(unif1);
			unif1->uniformName->setText( QString::fromStdString(name) );
			unif1->uniformChoice->setCurrentIndex( value );

			uniform = uniform->NextSiblingElement();
		}
	}

	TiXmlElement* AttributeElement = hdl.FirstChildElement( "attributes" ).Element();
	if( AttributeElement != nullptr )
	{
		TiXmlElement* attribute = AttributeElement->FirstChildElement();
		while( attribute != nullptr )
		{
			int value = 0;
			attribute->QueryIntAttribute("value",&value);
			std::string name;
			attribute->QueryStringAttribute("name",&name);
	
			AttributeWidget* attrib1 = new AttributeWidget();
			attributesZone->addWidget(attrib1);
			addedAttributeWidgets.push_back(attrib1);
			attrib1->attributeName->setText( QString::fromStdString(name) );
			attrib1->attributeChoice->setCurrentIndex( value );

			attribute = attribute->NextSiblingElement();
		}
	}

	TiXmlElement* VertexShaderElement = hdl.FirstChildElement("vertex_shader").Element();
	if( VertexShaderElement != nullptr )
	{
		VertexShaderEdit->setText( QString( VertexShaderElement->GetText() ) );
	}

	TiXmlElement* FragmentShaderElement = hdl.FirstChildElement("fragment_shader").Element();
	if( FragmentShaderElement != nullptr )
	{
		FragmentShaderEdit->setText( QString( FragmentShaderElement->GetText() ) );
	}
}