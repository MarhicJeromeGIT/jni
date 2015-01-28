#include "VikingScene.h"
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

VikingScene::VikingScene(SceneManager* manager) : Scene(manager)
{
}

void VikingScene::init()
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

	Model* vikingModel = new Model();

#if BUILD_DATA
	Assimp::Importer importer;
	Assimp::Importer importer2;
	Assimp::Importer importer3;
	ModelImport modelImport, modelImport2, modelImport3;

	const aiScene* scene1 = importer.ReadFile( DATA_PATH "viking.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );

	if( !scene1  )
	{
		cout<<"error loading "<<"Beaubovin-lowpoly-animated-with-basic-rig.dae"<<endl;
		cout<<importer.GetErrorString();
		assert(false);
		return;
	}
	bool result = modelImport.Import( scene1, vikingModel );
	assert( result == true );
	vikingModel->Save( DATA_PATH "vikingModel.txt" );
	delete vikingModel;
	vikingModel = new Model();

#endif

	// tree model
	vikingModel->Load( DATA_PATH "vikingModel.txt" );
	OpenGLStaticModel* vikingGL = new OpenGLStaticModel( vikingModel );
	mat4 treeMatrix = glm::translate( mat4(1.0), vec3(0,0,0) ) * glm::rotate( mat4(1.0), -90.0f, vec3(1,0,0) );
	vikingInstance = new OpenGLStaticModelInstance( treeMatrix, vikingGL );

	viking_mesh_index = vikingInstance->getMeshIndex( "Viking" );
	assert( viking_mesh_index != -1 );

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
}

void VikingScene::deinit()
{

}

void VikingScene::update(float dt)
{
	ShaderParams::get()->time += dt;

	static float angle = 0.0f;
	static float speed = 20.0f;
	angle += speed * dt;
	if( angle > 45.0f || angle < -45.0f )
	{
		speed = -speed;
	}

	mat4 m = glm::translate( mat4(1.0), vec3(0.0,0.0,10.0) ) * glm::rotate( mat4(1.0), angle, vec3(0.0,1.0,0.0) ) * glm::translate( mat4(1.0), vec3(0.0,0.0,-10.0) );
	vikingInstance->sparePartMatrix[viking_mesh_index] = m;

}

void VikingScene::drawGrass()
{
	/*DrawCall drawcall;

	mat4 objectMat = mat4(1.0);

	drawcall.Pass     = COLOR;
	drawcall.modelMat = objectMat;
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	Shader* shader = grassMaterial->getShader(COLOR);
	if( shader != NULL )
	{
		drawcall.material          = grassMaterial;
		drawcall.mesh              = &(grassBlades->mesh);
		drawcall.transparencyMode  = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
		drawcall.hasTransparency   = true;
		drawcall.disableDepthWrite = false;
		Renderer::get()->draw( drawcall );
	}*/
}

void VikingScene::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	vikingInstance->Draw(COLOR);
	skybox->Draw(COLOR);


	Renderer::get()->endFrame();
}


