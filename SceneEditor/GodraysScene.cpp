#include "GodraysScene.h"
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

vec3 lightPosition = vec3(-5.0,5.0,20.0);

FramebufferObject* fbo = NULL;
QOpenGLFramebufferObject* fbo2 = NULL;

const int fboN = 512;
glm::vec2 lightposScreen;

GodrayScene::GodrayScene(SceneManager* manager) : Scene(manager)
{
	decay    = 0.95f;
	weight   = 0.5f;
	exposure = 1.0f;

}

void GodrayScene::init()
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
	Model* sphereModel = new Model();
#if BUILD_DATA
	Assimp::Importer importer;
	Assimp::Importer importer2;
	ModelImport modelImport, modelImport2;

	const aiScene* scene1 = importer.ReadFile( DATA_PATH "sphere.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	const aiScene* scene2 = importer2.ReadFile( DATA_PATH "Beaubovin-lowpoly-animated-with-basic-rig.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	if( !scene1 || !scene2)
	{
		cout<<"error loading "<<"Beaubovin-lowpoly-animated-with-basic-rig.dae"<<endl;
		cout<<importer.GetErrorString();
		assert(false);
		return;
	}
	bool result = modelImport.Import( scene1, sphereModel );
	assert( result == true );
	sphereModel->Save( DATA_PATH "sphere.txt" );

	result = modelImport2.Import( scene2, model );
	assert( result == true );
	model->Save( DATA_PATH "cowmodel.txt" );

#endif

	model->Load( DATA_PATH "cowmodel.txt" );
	OpenGLSkinnedModel* dwarfAnimGL = new OpenGLSkinnedModel( model );
	mat4 dwarfMatrix = glm::translate( mat4(1.0), vec3(0,-1,0) ) * glm::rotate( mat4(1.0), -0.0f, vec3(1,0,0) );
	animatedModel = new OpenGLSkinnedModelInstance( dwarfMatrix, dwarfAnimGL );
	assert( animatedModel->animation != NULL );
	animatedModel->animation->start();

	sphereModel->Load( DATA_PATH "sphere.txt" );
	OpenGLStaticModel* sphereStatic = new OpenGLStaticModel( sphereModel );
	mat4 lightMat = glm::translate( mat4(1.0), lightPosition );
	lightModel = new OpenGLStaticModelInstance( lightMat, sphereStatic );

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


	vec4* data = new vec4[fboN*fboN];
	for( int i=0; i< fboN*fboN; i++ )
	{
		data[i] = vec4(0.0,0.0,0.0,1.0);
	}
	GLuint texindex;
	glGenTextures(1, &texindex );
	glBindTexture(GL_TEXTURE_2D, texindex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fboN, fboN, 0, GL_RGBA, GL_FLOAT, data);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glBindTexture( GL_TEXTURE_2D, 0 );



	fbo2 = new QOpenGLFramebufferObject( fboN, fboN, QOpenGLFramebufferObject::Depth );

	TextureGL* lightShaft = new TextureGL( fbo2->texture() );
	TextureGL* lightDepth = TextureManager::get()->createDepthTexture( "lightDepth", 512, 512 );

	// FBO
	fbo = new FramebufferObject();
	fbo->AttachTexture(GL_TEXTURE_2D, texindex, GL_COLOR_ATTACHMENT0);
	fbo->Disable();

	delete[] data;

	fboImage = new Image( vec3(0.15,0.15,0.0), lightShaft );
	fboImage->scale = vec3(0.15,0.15,1.0);

	godrayShader = new GodrayShader();
	godrayShader->load();

	fullscreenQuad = new DynamicMesh(4,2);
	vec3 vert[] = { vec3(0.0,0.0,-10), vec3(1.0,0.0,-10), vec3(1.0,1.0,-10), vec3(0.0,1.0,-10) };
	vec2 uv[]   = { vec2(0.0,0.0), vec2(1.0,0.0), vec2(1.0,1.0), vec2(0.0,1.0) };
	unsigned short tri[2*3] = { 0,1,2, 0,2,3 };
	fullscreenQuad->updateGeometry( vert, uv, 4, tri, 2  );


	QIntTweakable* lightXTweakable = new QIntTweakable( "pos light X",
		[](int p){ 
			cout<<"LIGHT : "<<p<<endl; 
			ShaderParams::get()->lights[0].lightPosition.x = ((float) p)/100.0f; 
		},
		ShaderParams::get()->lights[0].lightPosition.x * 100.0f, -200.0f *100.0f, 200.0f *100.0f);
	emit tweakableAdded( lightXTweakable );

	QIntTweakable* decayTweakable = new QIntTweakable( "decay",
		[this](int p){ 
			this->decay = ((float) p)/100.0f; 
		},
		this->decay * 100.0f, 0.0f, 200.0f );
	emit tweakableAdded( decayTweakable );

	QIntTweakable* weightTweakable = new QIntTweakable( "weight",
		[this](int p){ 
			weight = ((float) p)/100.0f; 
		},
		weight * 100.0f, 0.0f, 200.0f );
	emit tweakableAdded( weightTweakable );

	QIntTweakable* exposureTweakable = new QIntTweakable( "exposure",
		[this](int p){ 
			exposure = ((float) p)/100.0f; 
		},
		exposure * 100.0f, 0.0f, 200.0f );
	emit tweakableAdded( exposureTweakable );	
}

void GodrayScene::deinit()
{

}

void GodrayScene::update(float dt)
{
	animatedModel->update(dt);

	// compute the light position in screen space :
	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	vec3 pos = glm::project( lightPosition, ShaderParams::get()->viewMatrix, ShaderParams::get()->projectionMatrix, vec4( 0, 0, ShaderParams::get()->win_x, ShaderParams::get()->win_y) );

	lightposScreen = glm::vec2( pos.x / ShaderParams::get()->win_x, pos.y / ShaderParams::get()->win_y );

	drawFBO();
}

void GodrayScene::drawFBO()
{
//	fbo->Bind();
	fbo2->bind();
	
	glViewport( 0, 0, fboN, fboN );
	glClearColor(0.0,0.0, 0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);
	
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	animatedModel->Draw(COLOR);
	lightModel->Draw(COLOR);

	Renderer::get()->endFrame();

//	fbo->Disable();
	fbo2->release();

	glViewport( 0, 0, ShaderParams::get()->win_x, ShaderParams::get()->win_y );
}

void GodrayScene::drawLightShaft()
{
	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.1f,1000.0f);
	ShaderParams::get()->viewMatrix = mat4(1.0);
	ShaderParams::get()->objectMatrix = mat4(1.0);
	
	godrayShader->enable( *ShaderParams::get() );
	godrayShader->uTextureSampler->setValue( fbo2->texture() );
	godrayShader->uLightposScreen->setValue( lightposScreen );
	godrayShader->uDecay->setValue(decay);
	godrayShader->uExposure->setValue(exposure);
	godrayShader->uWeight->setValue(weight);
	godrayShader->Draw( &(fullscreenQuad->mesh) );
	godrayShader->disable();
}

void GodrayScene::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	animatedModel->Draw(COLOR);
	lightModel->Draw(COLOR);
	skybox->Draw(COLOR);
	
	Renderer::get()->endFrame();

	// Composite
	drawLightShaft();

	Renderer::get()->beginFrame();
	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.1f,1000.0f);
	ShaderParams::get()->viewMatrix = mat4(1.0);
	fboImage->Draw(GUI);
	Renderer::get()->endFrame();
}


