#include "InstancingScene.h"
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
#define BUILD_DATA 1
#include "macros.h"

InstancingScene::InstancingScene(SceneManager* manager) : Scene(manager)
{
}

void InstancingScene::init()
{
	sceneManager->camera->position = glm::vec3(0,0,-10);

	Model* fenceModel = NULL; 
#if BUILD_DATA
	fenceModel = new Model();
	Assimp::Importer importer;
	ModelImport modelImport;

	const aiScene* scene1 = importer.ReadFile( DATA_PATH "cube.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	if( !scene1 )
	{
		cout<<"error loading "<<"Beaubovin-lowpoly-animated-with-basic-rig.dae"<<endl;
		cout<<importer.GetErrorString();
		assert(false);
		return;
	}
	bool result = modelImport.Import( scene1, fenceModel );
	assert( result == true );
	fenceModel->Save( DATA_PATH "cube.txt" );
	delete fenceModel;
#endif

	// tree model
	fenceModel = new Model();
	fenceModel->Load( DATA_PATH "cube.txt" );
	OpenGLStaticModel* fenceGL = new OpenGLStaticModel( fenceModel );
	mat4 fenceMatrix = glm::translate( mat4(1.0), vec3(0,0,0) ) * glm::rotate( mat4(1.0), 0.0f, vec3(1,0,0) );
	fenceInstance = new OpenGLStaticModelInstance( fenceMatrix, fenceGL );

	// Instance Material :
	instanceMat = new MaterialInstanceStatic();
	fenceInstance->materials[0] = instanceMat;

	const int cubeSide = 10;
	const int nbCubes = cubeSide * cubeSide * cubeSide;
	fenceInstance->glModel->meshes[0].nbInstances = nbCubes;
	glm::mat4 matrices[nbCubes];
	for( int i=0; i< cubeSide; i++ )
	{
		for( int j=0; j < cubeSide; j++ )
		{
			for( int k=0; k < cubeSide; k++ )
			{
				float offset = 5.0f * cubeSide / 2.0f;
				matrices[i*cubeSide*cubeSide + j*cubeSide + k] = glm::translate( mat4(1.0), vec3(i*5 - offset,j*5 - offset,k*5 - offset) ) * glm::rotate( mat4(1.0), 0.0f, vec3(1,0,0) );
			}
		}
	}
	glGenBuffers(1, &( fenceInstance->glModel->meshes[0].modelMatrixBuffer ) );
	glBindBuffer(GL_ARRAY_BUFFER, fenceInstance->glModel->meshes[0].modelMatrixBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * nbCubes, matrices, GL_DYNAMIC_DRAW);

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
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(24,41,-50,1.0);
	ShaderParams::get()->lights[0].lightDiffuseColor   = glm::vec4(1.0,1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightSpecularColor  = glm::vec3(1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightShininess      = 32;

	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
}

void InstancingScene::deinit()
{

}

void InstancingScene::update(float dt)
{
}

void InstancingScene::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	fenceInstance->Draw(COLOR);

	Renderer::get()->endFrame();
}


