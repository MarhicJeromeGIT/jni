#include "SkeletalAnimationScene.h"
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
#include "Animation.h"
#include "SceneManager.h"

#define BUILD_DATA 1

SkeletalAnimationScene::SkeletalAnimationScene(SceneManager* manager) : Scene(manager)
{
}

void SkeletalAnimationScene::init()
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

	Model* model = new Model();
#if BUILD_DATA
	Assimp::Importer importer;
	ModelImport modelImport;

	const aiScene* scene2 = importer.ReadFile( DATA_PATH "Beaubovin-lowpoly-animated-with-basic-rig.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	if( !scene2)
	{
		cout<<"error loading "<<"Beaubovin-lowpoly-animated-with-basic-rig.dae"<<endl;
		cout<<importer.GetErrorString();
		assert(false);
		return;
	}
	bool result = modelImport.Import( scene2, model );
	assert( result == true );
	model->Save( DATA_PATH "cowmodel.txt" );

#endif

	model->Load( DATA_PATH "cowmodel.txt" );
	OpenGLSkinnedModel* dwarfAnimGL = new OpenGLSkinnedModel( model );
	mat4 dwarfMatrix = glm::translate( mat4(1.0), vec3(0,-1,0) ) * glm::rotate( mat4(1.0), -0.0f, vec3(1,0,0) );
	animatedModel = new OpenGLSkinnedModelInstance( dwarfMatrix, dwarfAnimGL );
	assert( animatedModel->animation != NULL );
	animatedModel->animation->start();

	// Skybox
	Model* skyboxModel = new Model();
	skyboxModel->Load( DATA_PATH "skybox.txt" );
	OpenGLStaticModel* skyboxGL = new OpenGLStaticModel(skyboxModel);
	std::string filename[6] = { DATA_PATH "skybox_texture_right.png",  DATA_PATH "skybox_texture_left.png",
							    DATA_PATH "skybox_texture_top.png",  DATA_PATH "skybox_texture_bottom.png",
								DATA_PATH "skybox_texture_front.png",  DATA_PATH "skybox_texture_back.png" };
	skybox  = new Skybox(skyboxGL, filename);

	// Global lighting parameters
	ShaderParams::get()->ambient                       = vec4(0.5,0.5,0.5,1.0);
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(0,20,-5,1.0);
	ShaderParams::get()->lights[0].lightDiffuseColor   = glm::vec4(1.0,1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightSpecularColor  = glm::vec3(1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightShininess      = 32;
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(0,15,-50,1.0);


	QIntTweakable* lightXTweakable = new QIntTweakable( "pos light X",
		[](int p){ 
			cout<<"LIGHT : "<<p<<endl; 
			ShaderParams::get()->lights[0].lightPosition.x = ((float) p)/100.0f; 
		},
		ShaderParams::get()->lights[0].lightPosition.x * 100.0f, -200.0f *100.0f, 200.0f *100.0f);
	emit tweakableAdded( lightXTweakable );

}

void SkeletalAnimationScene::deinit()
{

}

void SkeletalAnimationScene::update(float dt)
{
	animatedModel->update(dt);
}

void SkeletalAnimationScene::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	animatedModel->Draw(COLOR);
	skybox->Draw(COLOR);

	Renderer::get()->endFrame();
}


