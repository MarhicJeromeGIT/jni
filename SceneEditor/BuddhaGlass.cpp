#include "BuddhaGlass.h"
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

BuddhaGlass::BuddhaGlass( SceneManager* manager ) : Scene( manager )
{
}

void BuddhaGlass::init()
{

	Model* buddhaModel = new Model();
	Model* skyboxModel = new Model();
#if DATA_BUILD
	Assimp::Importer importer, importer2;
	const aiScene* scene = importer.ReadFile( DATA_PATH "buddha.obj", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	const aiScene* scene2 = importer2.ReadFile( DATA_PATH "cube.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );

	if( !scene || !scene2 )
	{
		cout<<"error loading "<<"buddha.obj"<<endl;
		cout<<importer.GetErrorString();
		assert(false);
		return;
	}
	ModelImport modelImport, modelImport2;
	bool result = modelImport.Import( scene, buddhaModel );
	assert( result == true );
	buddhaModel->Save( DATA_PATH "Buddha.txt" );
	delete buddhaModel;
	buddhaModel = new Model();

	result = modelImport2.Import( scene2, skyboxModel );
	assert( result == true );
	skyboxModel->Save( DATA_PATH "skybox.txt" );
	delete skyboxModel;
	skyboxModel = new Model();
#endif
	buddhaModel->Load( DATA_PATH "Buddha.txt" );
	OpenGLStaticModel* buddhaGL = new OpenGLStaticModel( buddhaModel );

	buddha_angle = 0.0f;
	glm::mat4 buddhamat = glm::translate( mat4(1.0), vec3(0.0,-5.0,0.0) ) * glm::rotate( glm::mat4(1.0), buddha_angle, glm::vec3(0,1,0) );
	buddha = new OpenGLStaticModelInstance(buddhamat, buddhaGL );

	skyboxModel = new Model();
	skyboxModel->Load( DATA_PATH "skybox.txt" );
	OpenGLStaticModel* skyboxGL = new OpenGLStaticModel(skyboxModel);
	std::string filename[6] = { DATA_PATH "skybox_texture_right.png",  DATA_PATH "skybox_texture_left.png",
							    DATA_PATH "skybox_texture_top.png",  DATA_PATH "skybox_texture_bottom.png",
								DATA_PATH "skybox_texture_front.png",  DATA_PATH "skybox_texture_back.png" };
	skybox  = new Skybox(skyboxGL, filename);


	buddhaMat = new MaterialGlass();
	TextureGL* vide = TextureManager::get()->createRGBATexture( "vide", vec4(0.5,0.5,0.5,1.0), 64, 64 );
	buddhaMat->setTextureDiffuse( vide );
	buddhaMat->setTextureNormal( vide );
	buddhaMat->setCubemapTex( skybox->cubemap );
	refractionIndex = 1.05f;
	buddhaMat->setRefractIndex( refractionIndex );

	buddha->setMaterial( "wire_134110008", buddhaMat );

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
			cout<<"Light color changed"<<endl;
		},
		ShaderParams::get()->lights[0].lightDiffuseColor );
	emit tweakableAdded( lightColorTweakable );

	drawSky = true;
	QBooleanTweakable* showSkyTweakable = new QBooleanTweakable( "draw sky",
		[this](bool draw){
			drawSky = draw;
	    }, drawSky );
	emit tweakableAdded( showSkyTweakable );

	QFloatTweakable* refractTweakable = new QFloatTweakable( "refraction index",
		[this](float f){
			refractionIndex = f;
			buddhaMat->setRefractIndex( refractionIndex );
	}, refractionIndex, 1.0f, 3.0f );
	emit tweakableAdded( refractTweakable );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void BuddhaGlass::deinit()
{

}

void BuddhaGlass::update(float dt)
{

}

void BuddhaGlass::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	buddha->Draw( COLOR );

	if( drawSky )
	{
		skybox->Draw( COLOR );
	}

	Renderer::get()->endFrame();
}
