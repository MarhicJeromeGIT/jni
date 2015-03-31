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
#include "UniformWidget.h"
#include <qlabel.h>

//**********************************
// ShaderEditorWidget 
//
//**********************************
ShaderEditorWidget::ShaderEditorWidget( ) : QWidget()
{
	mainLayout = new QVBoxLayout;
	this->setLayout( mainLayout );
	this->setWindowTitle( "Shader Creator" );

	QGLFormat format;
	cout<<format.openGLVersionFlags();

	format.setVersion(2,1);
	format.setProfile( QGLFormat::CompatibilityProfile );
	format.setDoubleBuffer(true);
	format.setSampleBuffers( true );

	QHBoxLayout* topButtonsLayout = new QHBoxLayout();
	QPushButton* saveButton = new QPushButton( QString("Save") );
	QPushButton* openButton = new QPushButton( QString("Open") );
	QPushButton* compileButon = new QPushButton( QString("Compile"));
	QPushButton* sphereButton = new QPushButton( QString("Sphere") );
	QPushButton* planeButton = new QPushButton( QString("Plane") );
	topButtonsLayout->addWidget( saveButton );
	topButtonsLayout->addWidget( openButton );
	topButtonsLayout->addWidget( compileButon );
	topButtonsLayout->addWidget( sphereButton );
	topButtonsLayout->addWidget( planeButton );
	connect( saveButton,   &QPushButton::clicked, this, [this](){ saveCurrentState(); } );
	connect( openButton,   &QPushButton::clicked, this, [this](){ openCurrentState(); } );
	connect( sphereButton, &QPushButton::clicked, this, [this](){ shaderScene->setMesh(EDITOR_MESH_TYPE::EDITOR_SPHERE_MESH); } );
	connect( planeButton,  &QPushButton::clicked, this, [this](){ shaderScene->setMesh(EDITOR_MESH_TYPE::EDITOR_PLANE_MESH); } );

	// Setup the interface (TODO : doable with qt editor ?)
	mainLayout->addLayout( topButtonsLayout );
	
	QSplitter* splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Orientation::Horizontal);
	mainLayout->addWidget( splitter );

	// LEFT SIDE : opengl view + uniforms/attributes zones
	QSplitter* leftSplitter = new QSplitter(this);
	leftSplitter->setOrientation(Qt::Orientation::Vertical);
	splitter->addWidget( leftSplitter );
	
	// the opengl view
	glWidget = new GLWidget(format);
	leftSplitter->addWidget(glWidget);

	// attribute & uniform zone
	
	uniformsZone = new QVBoxLayout();
	//uniformsZone->setSpacing(20);
	attributesZone = new QVBoxLayout();

	tweakableLayout = new QVBoxLayout;
	tweakableLayout->setAlignment( Qt::AlignTop );
	QScrollArea *leftSide = new QScrollArea();  
	leftSide->setWidgetResizable(true);
	//leftSide->setLayout( tweakableLayout );
	leftSplitter->addWidget(leftSide);
	
	QWidget* l = new QWidget();
	leftSide->setWidget(l);
	l->setLayout( tweakableLayout );

	tweakableLayout->addLayout(uniformsZone);
	tweakableLayout->addLayout(attributesZone);


	// Right side : Vertex and fragment shader textedit
	QSplitter* splitterRightside = new QSplitter(this);
	splitterRightside->setOrientation(Qt::Orientation::Vertical);
	splitter->addWidget( splitterRightside );

	VertexShaderEdit = new QTextEdit();
	FragmentShaderEdit = new QTextEdit();

	splitterRightside->addWidget(VertexShaderEdit);
	splitterRightside->addWidget(FragmentShaderEdit);

	glWidget->setScene("ShaderEditorScene");
	shaderScene = (ShaderEditorScene*) glWidget->getScene("ShaderEditorScene");
	assert( shaderScene != nullptr );

	connect( compileButon, SIGNAL( clicked() ), this, SLOT( compileShader() ) );
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
	UniformWidget* unif1 = new UniformWidget( this );
	uniformsZone->addWidget(unif1);
	addedUniformWidgets.push_back(unif1);
	unif1->uniformName->setText( QString("ModelMatrix") );
	unif1->uniformChoice->setCurrentIndex( (int) MODEL_MATRIX );
	UniformWidget* unif2 = new UniformWidget( this );
	uniformsZone->addWidget(unif2);
	addedUniformWidgets.push_back(unif2);
	unif2->uniformName->setText( QString("ViewMatrix") );
	unif2->uniformChoice->setCurrentIndex( (int) VIEW_MATRIX );
	UniformWidget* unif3 = new UniformWidget( this );
	uniformsZone->addWidget(unif3);
	addedUniformWidgets.push_back(unif3);
	unif3->uniformName->setText( QString("ProjectionMatrix") );
	unif3->uniformChoice->setCurrentIndex( (int) PROJECTION_MATRIX );

	connect( addUniformButton,  &QPushButton::clicked, this, [this]() { 
		UniformWidget* newUnif = new UniformWidget( this );
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

void ShaderEditorWidget::removeWidget( UniformWidget* widget )
{
	uniformsZone->removeWidget( widget );
	auto it = std::find( addedUniformWidgets.begin(), addedUniformWidgets.end(), widget );
	if( it != addedUniformWidgets.end() )
	{
		addedUniformWidgets.erase(it);
	}
	delete widget;
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

void ShaderEditorWidget::setUniformTexture( UNIFORM_TYPE type, const QString& filename )
{
	TextureManager::get()->setTexturePath("");
	TextureGL* tex = TextureManager::get()->loadTexture(filename.toStdString(), filename.toStdString() );
	
	map<std::string,UNIFORM_TYPE> aliasToUniformTypeMap;
	BuildUniformMap( aliasToUniformTypeMap);

	shaderScene->updateTexture(aliasToUniformTypeMap, type, tex );
}


void ShaderEditorWidget::saveCurrentState()
{
	QString filename = QFileDialog::getSaveFileName(0,"","",QString("*.xml"));
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

		switch(it->second) // save the value of the uniform (texture name for texture type, value for float, etc)
		{
		case UNIFORM_TYPE::TEXTURE2D_0:
			newUniform->SetAttribute("filename", shaderScene->customParams.tex0->filename );
			break;
		case UNIFORM_TYPE::TEXTURE2D_1:
			newUniform->SetAttribute("filename", shaderScene->customParams.tex1->filename );
			break;
		case UNIFORM_TYPE::TEXTURE2D_2:
			newUniform->SetAttribute("filename", shaderScene->customParams.tex2->filename );
			break;
		case UNIFORM_TYPE::TEXTURE2D_3:
			newUniform->SetAttribute("filename", shaderScene->customParams.tex3->filename );
			break;
		}
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
	QString filename = QFileDialog::getOpenFileName(0,"","",QString("*.xml"));

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
	
			UniformWidget* unif1 = new UniformWidget( this );
			uniformsZone->addWidget(unif1);
			addedUniformWidgets.push_back(unif1);
			unif1->uniformName->setText( QString::fromStdString(name) );
			unif1->uniformChoice->setCurrentIndex( value );

			switch( (UNIFORM_TYPE)value ) // save the value of the uniform (texture name for texture type, value for float, etc)
			{
			case UNIFORM_TYPE::TEXTURE2D_0:
			case UNIFORM_TYPE::TEXTURE2D_1:
			case UNIFORM_TYPE::TEXTURE2D_2:
			case UNIFORM_TYPE::TEXTURE2D_3:
				std::string name;
				uniform->QueryStringAttribute("filename",&name);
				unif1->setImage( name );
				break;
			}

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