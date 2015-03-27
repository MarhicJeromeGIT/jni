#include "ShaderEditorScene.h"
#define QT_NO_OPENGL_ES_2
#include "GL\glew.h"
#include "Shader.h"

#include <iostream>
using namespace std;
using glm::vec3;
using glm::mat4;
using glm::vec4;

#include <QOpenGLFramebufferObject>
#include "AntigravityConfig.h"

#include "Model.h"
#include "ModelImport.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp> // C++ importer interface
#include "OpenGLModel.h"
#include "Camera.h"
#include "QTweakable.h"
#include "Skybox.h"
#include "TextureGL.h"
#include "Animation.h"
#include "SceneManager.h"
#include "FramebufferObject.h"
#include "Gui\Image.h"
#include "GodrayShader.h"
#include "macros.h"
#include "OpenGLModel.h"
#include <map>

ShaderEditorScene::ShaderEditorScene(SceneManager* manager) : Scene(manager)
{
}

void ShaderEditorScene::init()
{
	// GLEW d'abord
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// Problem: glewInit failed, something is seriously wrong. 
		cout<< "Error: "<<glewGetErrorString(err)<<endl;
		system("pause");
		exit(-1);
	}
	if ( !glewIsSupported("GL_VERSION_4_2"))
	{
		cout<<"achete une nouvelle carte !"<<endl;
		//system("pause");
		//exit(-1);
	}
	cout<<"Status: Using GLEW "<<glewGetString(GLEW_VERSION)<<endl;

	sceneManager->camera->position = glm::vec3(0,0,-10);


	// Skybox
	Model* skyboxModel = new Model();
	skyboxModel->Load( DATA_PATH "skybox.txt" );
	OpenGLStaticModel* skyboxGL = new OpenGLStaticModel(skyboxModel);
	std::string filename[6] = { DATA_PATH "skybox_texture_right.png",  DATA_PATH "skybox_texture_left.png",
							    DATA_PATH "skybox_texture_top.png",  DATA_PATH "skybox_texture_bottom.png",
								DATA_PATH "skybox_texture_front.png",  DATA_PATH "skybox_texture_back.png" };
	skybox  = new Skybox(skyboxGL, filename);

	// Sphere mesh
	Model* sphereModel = new Model();
	sphereModel->Load( DATA_PATH "sphereModel.txt" );
	OpenGLStaticModel* sphereGL = new OpenGLStaticModel(sphereModel);
	mesh = new OpenGLStaticModelInstance( glm::translate(mat4(1.0), vec3(0.0,0.5,0.0) ), sphereGL );

	customMat = new CustomMaterial();
	mesh->setMaterial(0,customMat);

	// Global lighting parameters
	ShaderParams::get()->ambient                       = vec4(0.5,0.5,0.5,1.0);
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(0,20,-5,1.0);
	ShaderParams::get()->lights[0].lightDiffuseColor   = glm::vec4(1.0,1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightSpecularColor  = glm::vec3(1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightShininess      = 32;
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(0,15,-50,1.0);

	UniformTypeNames.insert( pair<std::string,UNIFORM_TYPE>("View Matrix",VIEW_MATRIX) );
	UniformTypeNames.insert( pair<std::string,UNIFORM_TYPE>("Object Matrix",MODEL_MATRIX) );
	UniformTypeNames.insert( pair<std::string,UNIFORM_TYPE>("Projection Matrix",PROJECTION_MATRIX) );
	UniformTypeNames.insert( pair<std::string,UNIFORM_TYPE>("Normal Matrix",NORMAL_MATRIX) );

}

void ShaderEditorScene::deinit()
{

}


void ShaderEditorScene::update(float dt)
{
}


void ShaderEditorScene::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	mesh->Draw(COLOR);
	skybox->Draw(COLOR);

	Renderer::get()->endFrame();

}


//********************
// SHADER AND MATERIAL
//
//********************

MyShader::MyShader()
{
	// Default basic shader
	string vs = "uniform mat4 ModelMatrix; \n \
				 uniform mat4 ViewMatrix; \n \
				 uniform mat4 ProjectionMatrix; \n \
				 in vec3  vertexPosition; \n \
				 void main(void) \n \
				 {	 \n \
					gl_Position  = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertexPosition,1.0); \n \
				 }\n";
	string ps = "void main(void)  \n \
				{  \n \
					gl_FragColor = vec4(0.3,0.3,0.3,1.0); \n \
				} \n";

	LoadShaderFromFile(vs,ps);

	uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
	assert(uModelMat != 0);

	uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
	assert(uViewMat != 0);

	uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
	assert(uProjectionMat != 0);

	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );
}

// return true if no compilation error
bool MyShader::compile(const std::string& vertexSource, const std::string& fragmentSource, map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap)
{
	bool compiled = newCompileProgram( vertexSource, fragmentSource );
	if( !compiled ) return false;

	GetUniforms();

	for( auto it = _uniforms.begin(); it != _uniforms.end(); it++ )
	{
		cout<<it->first<<endl;
	}

	myUniformVector.clear();

/*	auto castUniformMat4f = [this](Uniform* uniform, std::string& id)
	{
		cout<<"calling castUniformMat4f "<<endl;
		uniform = GetUniformByName(id);
	};
	auto castUniformFloat = [this](Uniform* myUniform, std::string& id)
	{
		myUniform = (UniformFloat*) GetUniformByName(id);
	};


	
	map<std::string,std::function<void (Uniform*)> > SetUniformMap;
	SetUniformMap.insert( pair<std::string,std::function<void (Uniform*)> >( myViewMatrixVar, std::bind(castUniformMat4f, std::placeholders::_1, myViewMatrixVar) ) );

	SetUniformMap[myViewMatrixVar](myUniform);
	*/
	//std::string myViewMatrixVar = "ViewMatrix";
	//myUniform = GetUniformByName(myViewMatrixVar);

	auto setValueMat4f = [](Uniform* uniform, glm::mat4* matPtr)
	{
		((UniformMat4f*)uniform )->setValue( *matPtr );
	};
	glm::mat4* viewMatrix       = &( ShaderParams::get()->viewMatrix );
	glm::mat4* objectMatrix     = &( ShaderParams::get()->objectMatrix );
	glm::mat4* projectionMatrix = &( ShaderParams::get()->projectionMatrix );

	map<UNIFORM_TYPE, std::function< void (Uniform*) > > setValueMap;

	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::VIEW_MATRIX, std::bind( setValueMat4f, std::placeholders::_1, viewMatrix ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::MODEL_MATRIX, std::bind( setValueMat4f, std::placeholders::_1, objectMatrix ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::PROJECTION_MATRIX, std::bind( setValueMat4f, std::placeholders::_1, projectionMatrix ) ) );

	for( auto it = aliasToUniformTypeMap.begin(); it != aliasToUniformTypeMap.end(); it++ )
	{
		Uniform* myUniform = NULL;
		myUniform = GetUniformByName( it->first );
		if( myUniform != NULL )
		{
			auto myFunc = std::bind( setValueMap[it->second], myUniform );
			myUniformVector.push_back( myFunc );
		}
	}	
		
	vertexAttribLoc = glGetAttribLocation( getProgram(), "vertexPosition" );

	return compiled;
}

void MyShader::Draw(Mesh* m)
{
	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
	glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}

void MyShader::enable( const ShaderParams& params )
{
	glUseProgram(_program);
	
	//uModelMat->setValue( params.objectMatrix );
	//uViewMat->setValue( params.viewMatrix );
	//uProjectionMat->setValue( params.projectionMatrix );
	glEnableVertexAttribArray(vertexAttribLoc);

	for( auto& it : myUniformVector )
	{
		it();
	}

	//UniformFloat
	
}

void MyShader::disable()
{
	glDisableVertexAttribArray(vertexAttribLoc);
	Shader::disable();
}

CustomMaterial::CustomMaterial() : Material(MATERIAL_SKINNING)
{
	shader = new MyShader();

} 

bool CustomMaterial::compile(const std::string& vertexSource, const std::string& fragmentSource,
							 map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap)
{
	return shader->compile( vertexSource, fragmentSource, aliasToUniformTypeMap );
}

const std::string& CustomMaterial::getErrorString()
{
	return shader->getErrorString();
}

void CustomMaterial::SetupUniforms(MATERIAL_DRAW_PASS Pass)
{
	//shader->uColorMap->setValue( colorMap->getTexId() );
}

Shader* CustomMaterial::getShader( MATERIAL_DRAW_PASS Pass )
{
	return shader;
}