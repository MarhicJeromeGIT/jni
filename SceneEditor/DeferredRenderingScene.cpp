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
#include "macros.h"

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

	floorModel = load_assimp_model( DATA_PATH "plane.dae" );
	floorModel->Save( DATA_PATH "floorModel.txt" );
	delete floorModel;
	
#endif

	// setup the skybox
	skyboxModel = new Model();
	skyboxModel->Load( DATA_PATH "skybox.txt" );
	OpenGLStaticModel* skyboxGL = new OpenGLStaticModel(skyboxModel);
	std::string filename[6] = { DATA_PATH "skybox_texture_right.png",  DATA_PATH "skybox_texture_left.png",
							    DATA_PATH "skybox_texture_top.png",  DATA_PATH "skybox_texture_bottom.png",
								DATA_PATH "skybox_texture_front.png",  DATA_PATH "skybox_texture_back.png" };
	skybox  = new Skybox(skyboxGL, filename);

	// Load various objects for the scene and give them my custom material
	sphereModel = new Model();
	sphereModel->Load( DATA_PATH "sphereModel.txt" );
	OpenGLStaticModel* sphereGL = new OpenGLStaticModel(sphereModel);
	sphereInstance = new OpenGLStaticModelInstance( glm::translate(mat4(1.0), vec3(0.0,0.5,0.0) ), sphereGL );

	MaterialDeferred* sphereDeferredMaterial = new MaterialDeferred();
	sphereDeferredMaterial->setTexture( sphereInstance->glModel->meshes[0].texture );
	sphereInstance->materials[0] = sphereDeferredMaterial;

	sceneManager->camera->position = glm::vec3(0,0,-10);

	floorModel = new Model();
	floorModel->Load( DATA_PATH "floorModel.txt" );
	OpenGLStaticModel* floorGL = new OpenGLStaticModel(floorModel);
	floorInstance = new OpenGLStaticModelInstance( glm::rotate( mat4(1.0), 0.0f, vec3(0.0f,0.0f,1.0f)), floorGL );

	MaterialDeferred* floorDeferredMaterial = new MaterialDeferred();
	floorDeferredMaterial->setTexture( floorInstance->glModel->meshes[0].texture );
	floorInstance->materials[0] = floorDeferredMaterial;

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
	QOpenGLFramebufferObject* GbufferQT3 = new QOpenGLFramebufferObject( 1024, 1024, fboFormat );
 	assert( GbufferQT3->isValid() );	
	QOpenGLFramebufferObject* GbufferQT4 = new QOpenGLFramebufferObject( 1024, 1024, fboFormat );
 	assert( GbufferQT4->isValid() );	

	GbufferViewer = new Image( vec3(0.20,0.75,-10), new TextureGL(GbufferQT->texture()) );
	GbufferViewer->scale = vec3(0.20,0.20,1.0);
	GbufferViewer2 = new Image( vec3(0.20,0.45,-10), new TextureGL(GbufferQT4->texture()) );
	GbufferViewer2->scale = vec3(0.20,0.20,1.0);
	
	//TextureGL* gbufferTex = TextureManager::get()->createRGBATexture("gbuffer", vec4(0.0,0.0,0.0,1.0), 1024, 1024 );
	TextureGL* gbufferDepthTex = TextureManager::get()->createDepthTexture("gbufferdepth", 1024, 1024 );
	TextureGL* lightbufferDepthTex = TextureManager::get()->createDepthTexture("ightbufferdepth", 1024, 1024 );

	// WORKING ! That means my texture are somehow not suitable for framebuffer use. Use
	// QT made texture for the time being.
	Gbuffer = new FramebufferObject();
	Gbuffer->AttachTexture(GL_TEXTURE_2D, GbufferQT->texture(), GL_COLOR_ATTACHMENT0);  // NORMAL
	Gbuffer->AttachTexture(GL_TEXTURE_2D, GbufferQT2->texture(), GL_COLOR_ATTACHMENT1); // VERTEX EYE POS
	Gbuffer->AttachTexture(GL_TEXTURE_2D, GbufferQT3->texture(), GL_COLOR_ATTACHMENT2); // DIFFUSE COLOR (AMBIENT)
	Gbuffer->AttachTexture(GL_TEXTURE_2D, gbufferDepthTex->getTexId(), GL_DEPTH_ATTACHMENT);
	Gbuffer->Disable();

	LightBuffer = new FramebufferObject();
	LightBuffer->AttachTexture(GL_TEXTURE_2D, GbufferQT4->texture(), GL_COLOR_ATTACHMENT0);  // NORMAL
	LightBuffer->AttachTexture(GL_TEXTURE_2D, lightbufferDepthTex->getTexId(), GL_DEPTH_ATTACHMENT);
	LightBuffer->Disable();


	// Quad for the fullscreen pass
	fullscreenQuad = new DynamicMesh(4,2);
	vec3 vert[] = { vec3(0.0,0.0,-10), vec3(1.0,0.0,-10), vec3(1.0,1.0,-10), vec3(0.0,1.0,-10) };
	vec2 uv[4]   = { vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,1) };
	unsigned short tri[2*3] = { 0,1,2, 0,2,3 };
	fullscreenQuad->updateGeometry( vert, uv, 4, tri, 2  );

	lightCombineMaterial = new MaterialLightCombine();
	lightCombineMaterial->setColorMap( new TextureGL( GbufferQT3->texture() ) );
	lightCombineMaterial->setLightMap( new TextureGL( GbufferQT4->texture() ) );
	
	// Setup the lighting of the scene
	ShaderParams::get()->lights[0].lightAttenuation = 1.0f;

	float lightDistance = 3.0f;
	float lightAngle = 0.0f;

	lightInstance = new OpenGLStaticModelInstance( mat4(1.0), sphereGL );

	TextureGL* normalMapTex = new TextureGL(GbufferQT->texture());
	TextureGL* vertexMapTex = new TextureGL(GbufferQT2->texture());
	
	float distance = 1.0f;
	lightInfo L1 = { vec4(distance,0.5,0.0,1.0), vec4(1.0,0.0,0.0,1.0), new MaterialDeferredFullScreen() };
	L1.fullscreenMaterial->setNormalMap(normalMapTex);
	L1.fullscreenMaterial->setVertexMap(vertexMapTex);
	lightInfoVector.push_back( L1 );
	lightInfo L2 = { vec4(distance*cos(M_PI*2.0/3.0),0.5,distance*sin(M_PI*2.0/3.0),1.0), vec4(0.0,1.0,0.0,1.0), new MaterialDeferredFullScreen() };
	L2.fullscreenMaterial->setNormalMap(normalMapTex);
	L2.fullscreenMaterial->setVertexMap(vertexMapTex);
	lightInfoVector.push_back( L2 );
	lightInfo L3 = { vec4(distance*cos(2.0*M_PI*2.0/3.0),0.5,distance*sin(2.0*M_PI*2.0/3.0),1.0), vec4(0.0,0.0,1.0,1.0), new MaterialDeferredFullScreen() };
	L3.fullscreenMaterial->setNormalMap(normalMapTex);
	L3.fullscreenMaterial->setVertexMap(vertexMapTex);
	lightInfoVector.push_back( L3 );

	// more white lights on the outside :
	for( int i=0; i< 6; i++ )
	{
		float angle = (((float)i)/6.0f ) * 2.0f * M_PI; 
		vec3 pos = vec3( 3.0 * cos(angle), 0.5, 3.0 * sin(angle) ); 
		lightInfo L = { vec4(pos,1.0), vec4(1.0,1.0,1.0,1.0), new MaterialDeferredFullScreen() };
		L.fullscreenMaterial->setNormalMap(normalMapTex);
		L.fullscreenMaterial->setVertexMap(vertexMapTex);
		lightInfoVector.push_back( L );
	}

	spherePositions.push_back( vec3(0.0,0.5,0.0) );
	int nbSPheres = 10;
	float rayon = 2.0;
	for( int i=0; i< 10; i++ )
	{
		float angle = (((float)i)/10.0f ) * 2.0f * M_PI; 
		vec3 pos = vec3( rayon * cos(angle), 0.5, rayon * sin(angle) ); 
		spherePositions.push_back( pos );
	}

	// Setup some tweakables.
/*	QIntTweakable* lightXTweakable = new QIntTweakable( "light angle",
		[this](int angle){ 
			 this->lightAngle = angle;
			ShaderParams::get()->lights[0].lightPosition.x = 3+cos(((float) this->lightAngle)*M_PI/180.0f) ;//*  this->lightDistance; 
			ShaderParams::get()->lights[0].lightPosition.z = 1+sin(((float) this-> lightAngle)*M_PI/180.0f);// *  this->lightDistance; 
		},
		0 , 0, 365);
	emit tweakableAdded( lightXTweakable );

	QIntTweakable* lightDistanceTweakable = new QIntTweakable( "light distance",
		[this](int distance){ 
			 this->lightDistance = ((float)distance)/100.0f;
			ShaderParams::get()->lights[0].lightPosition.y =  this->lightDistance;
			ShaderParams::get()->lights[0].lightPosition.x = 3+cos(((float)  this->lightAngle)*M_PI/180.0f);// *  this->lightDistance; 
			ShaderParams::get()->lights[0].lightPosition.z = 1+sin(((float)  this->lightAngle)*M_PI/180.0f);// *  this->lightDistance; 
		},
		lightDistance *100.0f , 0, 10.0f*100.0f);
	emit tweakableAdded( lightDistanceTweakable );

	QColorTweakable* lightColorTweakable = new QColorTweakable( "light color",
		[](vec4 color){ 
			ShaderParams::get()->lights[0].lightDiffuseColor = color; 
			cout<<"Light color changed"<<endl;
		},
		ShaderParams::get()->lights[0].lightDiffuseColor );
	emit tweakableAdded( lightColorTweakable );
*/

	QFloatTweakable* lightAttenuationTweakable = new QFloatTweakable("attenuation",
		[](float att){
			ShaderParams::get()->lights[0].lightAttenuation = att;
			cout<<"att = "<<att<<endl;
	},
	ShaderParams::get()->lights[0].lightAttenuation, 0.0f,4.0f );
	emit tweakableAdded( lightAttenuationTweakable );

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
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, buffers);

	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	// That's just a test.
	for( unsigned int i=0; i< spherePositions.size(); i++ )
	{
		sphereInstance->objectMatrix = glm::translate( mat4(1.0), spherePositions[i] );
		sphereInstance->Draw( COLOR );
	}
	floorInstance->Draw( COLOR );

	Renderer::get()->endFrame();

	Gbuffer->Disable();

	glViewport(0,0,ShaderParams::get()->win_x, ShaderParams::get()->win_y);
	glClearColor( 0.2f,0.2f,0.2f,1.0f );
}

void DeferredScene::fullscreenPass()
{
	// 1. Accumulate the lights in a buffer
	LightBuffer->Bind();
	glViewport(0,0,1024, 1024);
	glClearColor(0.0,0.0, 0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	Renderer::get()->beginFrame();

	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.0f,1000.0f);
	ShaderParams::get()->viewMatrix = mat4(1.0);
	ShaderParams::get()->objectMatrix = mat4(1.0);

	DrawCall drawcall;
	drawcall.Pass     = MATERIAL_DRAW_PASS::COLOR;
	drawcall.modelMat = mat4(1.0);
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	glBlendFunc(GL_ONE, GL_ONE);

	for( unsigned int i=0; i < lightInfoVector.size(); i++ )
	{
		lightInfo& light = lightInfoVector[i];

		light.fullscreenMaterial->setLightColor( light.Color );
		light.fullscreenMaterial->setLightPosition( light.Position );
		lightInstance->materials[0] = light.fullscreenMaterial;
		lightInstance->objectMatrix =  glm::translate( mat4(1.0), vec3(light.Position) ) * glm::scale(mat4(1.0), vec3(3.0,3.0,3.0) );

		Shader* shader = light.fullscreenMaterial->getShader(MATERIAL_DRAW_PASS::COLOR);
		if( shader != NULL )
		{
			drawcall.material = light.fullscreenMaterial;
			drawcall.mesh     = &(fullscreenQuad->mesh);
			drawcall.transparencyMode = TRANSPARENCY_MODE::GL_ONE_GL_ONE;
			drawcall.hasTransparency = true;
			drawcall.disableDepthWrite = true;
			Renderer::get()->draw( drawcall );
		}
	}

	Renderer::get()->endFrame();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LightBuffer->Disable();

	glViewport(0,0,ShaderParams::get()->win_x, ShaderParams::get()->win_y);
	glClearColor( 0.2f,0.2f,0.2f,1.0f );

	// 2. Combine the material color and light textures :
	Renderer::get()->beginFrame();

	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.0f,1000.0f);
	ShaderParams::get()->viewMatrix = mat4(1.0);
	ShaderParams::get()->objectMatrix = mat4(1.0);

	Shader* shader = lightCombineMaterial->getShader(MATERIAL_DRAW_PASS::COLOR);
	if( shader != NULL )
	{
		drawcall.material = lightCombineMaterial;
		drawcall.mesh     = &(fullscreenQuad->mesh);
		drawcall.transparencyMode = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
		drawcall.hasTransparency = true;
		drawcall.disableDepthWrite = true;
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
