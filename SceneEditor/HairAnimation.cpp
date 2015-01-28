#include "HairAnimation.h"
#define QT_NO_OPENGL_ES_2
#include "GL\glew.h"
#include "Shader.h"

#include <iostream>
using namespace std;
using glm::vec3;
using glm::mat4;
using glm::vec4;

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
#include "SceneManager.h"

#define DATA_BUILD 0

HairAnimation::HairAnimation( SceneManager* manager ) : Scene( manager )
{
}

void HairAnimation::init()
{
	Model* skyboxModel = new Model();
#if DATA_BUILD
	Assimp::Importer importer2;
	const aiScene* scene2 = importer2.ReadFile( DATA_PATH "cube.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );

	if( !scene2 )
	{
		cout<<"error loading "<<"cube.dae"<<endl;
		cout<<importer.GetErrorString();
		assert(false);
		return;
	}
	ModelImport modelImport2;
	bool result = modelImport2.Import( scene2, skyboxModel );
	assert( result == true );
	skyboxModel->Save( DATA_PATH "skybox.txt" );
	delete skyboxModel;
	skyboxModel = new Model();
#endif

	skyboxModel = new Model();
	skyboxModel->Load( DATA_PATH "skybox.txt" );
	OpenGLStaticModel* skyboxGL = new OpenGLStaticModel(skyboxModel);
	skybox  = new Skybox(skyboxGL, DATA_PATH);

	ShaderParams::get()->lights[0].lightPosition = glm::vec4(0,15,-50,1.0);
	ShaderParams::get()->lights[0].lightDiffuseColor = glm::vec4(1.0,1.0,1.0,1.0);

	sceneManager->camera->position = glm::vec3(0,0,-10);

	QIntTweakable* lightXTweakable = new QIntTweakable( "pos light X",
		[](int p){ 
			cout<<"LIGHT : "<<p<<endl; 
			ShaderParams::get()->lights[0].lightPosition.x = ((float) p)/100.0f; 
		},
		ShaderParams::get()->lights[0].lightPosition.x * 100.0f, -200.0f *100.0f, 200.0f *100.0f);
	emit tweakableAdded( lightXTweakable );

	QColorTweakable* lightColorTweakable = new QColorTweakable( "light color",
		[](vec4 color){ 
			ShaderParams::get()->lights[0].lightDiffuseColor = color; 
		},
		ShaderParams::get()->lights[0].lightDiffuseColor );
	emit tweakableAdded( lightColorTweakable );

	drawSky = true;
	QBooleanTweakable* showSkyTweakable = new QBooleanTweakable( "draw sky",
		[this](bool draw){
			drawSky = draw;
	    }, drawSky );
	emit tweakableAdded( showSkyTweakable );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void HairAnimation::deinit()
{

}

void HairAnimation::update(float dt)
{

}

void HairAnimation::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	if( drawSky )
	{
		skybox->Draw( COLOR );
	}

	Renderer::get()->endFrame();
}
