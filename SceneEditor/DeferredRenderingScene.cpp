#include "DeferredRenderingScene.h"
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
#include "FramebufferObject.h"
#include "Gui\Image.h"
#include "qopenglframebufferobject.h"

#define DATA_BUILD 1

DeferredScene::DeferredScene( SceneManager* manager ) : Scene( manager )
{
}

Model* load_assimp_model( string name )
{
	Assimp::Importer importer;
	const aiScene* assimpScene = importer.ReadFile( name, aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	assert(assimpScene);
	
	ModelImport modelImport;
	Model* model = new Model();

	bool result = modelImport.Import( assimpScene, model );
	assert( result == true );
	
	return model;
}

void DeferredScene::init()
{
	Model* sphereModel;
	Model* floorModel;
	Model* skyboxModel = new Model();

#if DATA_BUILD

	skyboxModel = load_assimp_model( DATA_PATH "cube.dae" );
	skyboxModel->Save( DATA_PATH "skybox.txt" );
	delete skyboxModel;

	sphereModel = load_assimp_model( DATA_PATH "sphere.obj" );
	sphereModel->Save( DATA_PATH "sphereModel.txt" );
	delete sphereModel;

#endif

	// setup the skybox
	skyboxModel = new Model();
	skyboxModel->Load( DATA_PATH "skybox.txt" );
	OpenGLStaticModel* skyboxGL = new OpenGLStaticModel(skyboxModel);
	std::string filename[6] = { DATA_PATH "skybox_texture_right.png",  DATA_PATH "skybox_texture_left.png",
							    DATA_PATH "skybox_texture_top.png",  DATA_PATH "skybox_texture_bottom.png",
								DATA_PATH "skybox_texture_front.png",  DATA_PATH "skybox_texture_back.png" };
	skybox  = new Skybox(skyboxGL, filename);

	// Load various objects for the scene
	sphereModel = new Model();
	sphereModel->Load( DATA_PATH "sphereModel.txt" );
	OpenGLStaticModel* sphereGL = new OpenGLStaticModel(sphereModel);
	sphereInstance = new OpenGLStaticModelInstance( mat4(1.0), sphereGL );
	sphereOriginalMaterial = sphereInstance->materials[0];

	deferredMaterial = new MaterialDeferred();


	sceneManager->camera->position = glm::vec3(0,0,-10);


	// this is the so-called "G buffer" we will draw the infos to.
	QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setMipmap(false);
    fboFormat.setSamples(0);
	fboFormat.setInternalTextureFormat(GL_RGBA32F);
	fboFormat.setAttachment( QOpenGLFramebufferObject::Attachment::Depth );

	GbufferQT= new QOpenGLFramebufferObject( 1024, 1024, fboFormat );
 	assert( GbufferQT->isValid() );	
	QOpenGLFramebufferObject* GbufferQT2= new QOpenGLFramebufferObject( 1024, 1024, fboFormat );
 	assert( GbufferQT2->isValid() );	

	GbufferViewer = new Image( vec3(0.20,0.75,-10), new TextureGL(GbufferQT->texture()) );
	GbufferViewer->scale = vec3(0.20,0.20,1.0);
	GbufferViewer2 = new Image( vec3(0.20,0.45,-10), new TextureGL(GbufferQT2->texture()) );
	GbufferViewer2->scale = vec3(0.20,0.20,1.0);

	//TextureGL* gbufferTex = TextureManager::get()->createRGBATexture("gbuffer", vec4(0.0,0.0,0.0,1.0), 1024, 1024 );
	TextureGL* gbufferDepthTex = TextureManager::get()->createDepthTexture("gbufferdepth", 1024, 1024 );

	// WORKING ! That means my texture are somehow not suitable for framebuffer use. Use
	// QT made texture for the time being.
	Gbuffer = new FramebufferObject();
	Gbuffer->AttachTexture(GL_TEXTURE_2D, GbufferQT->texture(), GL_COLOR_ATTACHMENT0);
	Gbuffer->AttachTexture(GL_TEXTURE_2D, GbufferQT2->texture(), GL_COLOR_ATTACHMENT1);
	Gbuffer->AttachTexture(GL_TEXTURE_2D, gbufferDepthTex->getTexId(), GL_DEPTH_ATTACHMENT);
	Gbuffer->Disable();

	// Quad for the fullscreen pass
	fullscreenQuad = new DynamicMesh(4,2);
	vec3 vert[] = { vec3(0.0,0.0,-10), vec3(1.0,0.0,-10), vec3(1.0,1.0,-10), vec3(0.0,1.0,-10) };
	vec2 uv[4]   = { vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,1) };
	unsigned short tri[2*3] = { 0,1,2, 0,2,3 };
	fullscreenQuad->updateGeometry( vert, uv, 4, tri, 2  );

	fullscreenMaterial = new MaterialDeferredFullScreen();
	fullscreenMaterial->setNormalMap( new TextureGL( GbufferQT->texture() ) );
	fullscreenMaterial->setVertexMap( new TextureGL( GbufferQT2->texture() ) );

	// Setup the lighting of the scene
	const float lightDistance = 10.0f;
	ShaderParams::get()->lights[0].lightPosition  = glm::vec4(lightDistance,15.0f,0.0f,1.0f);
	ShaderParams::get()->lights[0].lightDiffuseColor = glm::vec4(1.0,1.0,1.0,1.0);

	// Setup some tweakables.
	QIntTweakable* lightXTweakable = new QIntTweakable( "light angle",
		[lightDistance](int angle){ 
			ShaderParams::get()->lights[0].lightPosition.x = cos(((float) angle)*M_PI/180.0f) * lightDistance; 
			ShaderParams::get()->lights[0].lightPosition.z = sin(((float) angle)*M_PI/180.0f) * lightDistance; 
		},
		0 , 0, 365);
	emit tweakableAdded( lightXTweakable );

	QColorTweakable* lightColorTweakable = new QColorTweakable( "light color",
		[](vec4 color){ 
			ShaderParams::get()->lights[0].lightDiffuseColor = color; 
			cout<<"Light color changed"<<endl;
		},
		ShaderParams::get()->lights[0].lightDiffuseColor );
	emit tweakableAdded( lightColorTweakable );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DeferredScene::deinit()
{

}

void DeferredScene::update(float dt)
{

}

void DeferredScene::deferredPass()
{
	Gbuffer->Bind();
	glViewport(0,0,1024, 1024);
	glClearColor(0.0,0.0, 0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// MRT Multiple Render Target
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);

	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	// That's just a test.
	sphereInstance->materials[0] = deferredMaterial;
	sphereInstance->Draw( COLOR );
	sphereInstance->materials[0] = sphereOriginalMaterial;

	Renderer::get()->endFrame();

	Gbuffer->Disable();

	glViewport(0,0,ShaderParams::get()->win_x, ShaderParams::get()->win_y);
	glClearColor( 0.2f,0.2f,0.2f,1.0f );
}

void DeferredScene::fullscreenPass()
{
	Renderer::get()->beginFrame();

	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.0f,1000.0f);
	ShaderParams::get()->viewMatrix = mat4(1.0);
	ShaderParams::get()->objectMatrix = mat4(1.0);

	DrawCall drawcall;
	drawcall.Pass     = MATERIAL_DRAW_PASS::GUI;
	drawcall.modelMat = mat4(1.0);
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	Shader* shader = fullscreenMaterial->getShader(MATERIAL_DRAW_PASS::GUI);
	if( shader != NULL )
	{
		drawcall.material = fullscreenMaterial;
		drawcall.mesh     = &(fullscreenQuad->mesh);
		drawcall.transparencyMode = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
		drawcall.hasTransparency = true;
		Renderer::get()->draw( drawcall );
	}

	Renderer::get()->endFrame();
}

void DeferredScene::draw()
{
	// None deferred sky
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	skybox->Draw(COLOR);

	Renderer::get()->endFrame();

	// Deferred Pass
	// Write infos to the Gbuffer
	deferredPass();
	fullscreenPass();

	// 2D Pass
	glClear(GL_DEPTH_BUFFER_BIT);

	Renderer::get()->beginFrame();
	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.1f,1000.0f);
	ShaderParams::get()->viewMatrix = mat4(1.0);

	GbufferViewer->Draw(GUI);
	GbufferViewer2->Draw(GUI);

	Renderer::get()->endFrame();
}
