#include "BuddhaScene.h"


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
#include "FramebufferObject.h"
#include "Gui/Image.h"
#include "TextureGL.h"
#include "FullScreenEffectShader.h"
#include "SceneManager.h"

#define QT_NO_OPENGL_ES_2 true
#include <QOpenGLFramebufferObject>



int fbow = 0;
int fboh = 0;

BuddhaScene::BuddhaScene(SceneManager* manager) : Scene(manager)
{
	initialized = false;
	fboqt = NULL;
	fboqt2 = NULL;
	fboqt3 = NULL;
	myFbo = NULL;
	myFbo2 = NULL;
	myFboTex = 0;
	myFboTex2 = 0;

}

void BuddhaScene::init()
{

	glEnable(GL_DEPTH_TEST);

	Model* buddhaModel = new Model();
#if 0
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( DATA_PATH "buddha.obj", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	if( !scene)
	{
		cout<<"error loading "<<"buddha.obj"<<endl;
		cout<<importer.GetErrorString();
		assert(false);
		return;
	}
	ModelImport modelImport;
	buddhaModel = new Model();
	bool result = modelImport.Import( scene, buddhaModel );
	assert( result == true );
	buddhaModel->Save( DATA_PATH "Buddha.txt" );
#endif
	buddhaModel->Load( DATA_PATH "Buddha.txt" );
	OpenGLStaticModel* buddhaGL = new OpenGLStaticModel( buddhaModel );

	buddha_angle = 0.0f;
	glm::mat4 buddhamat = glm::translate( mat4(1.0), vec3(0.0,-5.0,0.0) ) * glm::rotate( glm::mat4(1.0), buddha_angle, glm::vec3(0,1,0) );
	buddha = new OpenGLStaticModelInstance(buddhamat, buddhaGL );


	// Global lighting parameters
	ShaderParams::get()->ambient                       = vec4(0.3,0.3,0.3,1.0);
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(0,15,-50,1.0);
	ShaderParams::get()->lights[0].lightDiffuseColor   = glm::vec4(1.0,1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightSpecularColor  = glm::vec3(1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightShininess      = 32;
	ShaderParams::get()->lights[0].lightPosition = glm::vec4(0,15,-50,1.0);

	sceneManager->camera->position = glm::vec3(0,0,-10);

	QIntTweakable* lightAngleTweakable = new QIntTweakable( "light Angle",
		[this](int p){ 
			float distance = glm::length( vec3(24,41,-50) ); 
			ShaderParams::get()->lights[0].lightPosition.x = cos( p / 100.0f ) * distance; 
			ShaderParams::get()->lights[0].lightPosition.z = sin( p / 100.0f ) * distance; 
		},
		ShaderParams::get()->lights[0].lightPosition.x * 100.0f, -2*M_PI *100.0f, 2*M_PI *100.0f);
	emit tweakableAdded( lightAngleTweakable );

	fbow = 1024;
	fboh = 1024;

	QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setMipmap(false);
    fboFormat.setSamples(0);
	fboFormat.setInternalTextureFormat(GL_RGBA32F);
	fboFormat.setAttachment( QOpenGLFramebufferObject::Attachment::Depth );

	fboqt = new QOpenGLFramebufferObject( fbow, fboh, fboFormat );
 	assert( fboqt->isValid() );	
	fboqt2 = new QOpenGLFramebufferObject( fbow, fboh, fboFormat );
	assert( fboqt2->isValid() );
	fboqt3 = new QOpenGLFramebufferObject( fbow, fboh, fboFormat );
	assert( fboqt3->isValid() );
/*
	glBindTexture( GL_TEXTURE_2D, fboqt->texture() );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fbow, fboh, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture( GL_TEXTURE_2D, fboqt2->texture() );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fbow, fboh, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture( GL_TEXTURE_2D, fboqt3->texture() );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fbow, fboh, 0, GL_RGBA, GL_FLOAT, NULL);
*/
	float* pixels = new float[ fbow * fboh * 4 ];
	for( int i=0; i < fbow * fboh; i++ )
	{
		pixels[4*i]   = 1.0f;
		pixels[4*i+1] = 0.5f;
		pixels[4*i+2] = 1.0f;
		pixels[4*i+3] = 1.0f;
	}

	glGenTextures(1, &myFboTex );
	glBindTexture( GL_TEXTURE_2D, myFboTex );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fbow, fboh, 0, GL_RGBA, GL_FLOAT, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glBindTexture( GL_TEXTURE_2D, 0 );

	glGenTextures(1, &myFboTex2 );
	glBindTexture( GL_TEXTURE_2D, myFboTex2 );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fbow, fboh, 0, GL_RGBA, GL_FLOAT, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glBindTexture( GL_TEXTURE_2D, 0 );

	myFbo = new FramebufferObject();
	myFbo->AttachTexture( GL_TEXTURE_2D, myFboTex, GL_COLOR_ATTACHMENT0 ); 
	myFbo->Disable();

	myFbo2 = new FramebufferObject();
	myFbo2->AttachTexture( GL_TEXTURE_2D, myFboTex2, GL_COLOR_ATTACHMENT0 ); 
	myFbo2->Disable();
	// Additive blending
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);

	TextureGL* tex = new TextureGL( fboqt->texture() ); 
	colorImage = new Image( vec3(0.20,0.75,-10), tex );
	colorImage->scale = vec3(0.30,0.30,1.0);

	TextureGL* tex2 = new TextureGL( fboqt2->texture() ); 
	colorImage2 = new Image( vec3(0.20,0.45,-10), tex2  );
	colorImage2->scale = vec3(0.30,0.30,1.0);

	TextureGL* tex3 = new TextureGL( fboqt3->texture() ); 
	colorImage3 = new Image( vec3(0.20,0.15,-10), tex3 );
	colorImage3->scale = vec3(0.30,0.30,1.0);

	fullscreenQuad = new DynamicMesh(4,2);
	vec3 vert[4] = { vec3(0,0,-10),vec3(1,0,-10), vec3(1,1,-10), vec3(0,1,-10) };
	vec2 uv[4]   = { vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,1) };
	unsigned short tri[2*3] = { 0,2,1, 0,3,2 };
	fullscreenQuad->updateGeometry( vert, uv, 4, tri, 2  );

	diffShader = (TextureMixShader*)ShaderManager::get()->getShader(TEXTURE_MIX_SHADER);
	assert(diffShader != NULL );

	//buddhaMat = new MaterialPhong();
	buddhaMat = new MaterialPhongWithDepth();
	buddhaMat->setTexture( tex3  );
	buddhaMat->setSigma( 80 );
	buddhaMat->setIntensity( 3.50f );
	buddha->setMaterial( "wire_134110008", buddhaMat );

	QIntTweakable* intensityTweakable = new QIntTweakable( "intensity",
		[this](int p){ 
			buddhaMat->setIntensity( ((float)p)/100.0f );
		},
		100.0f, 0.0f, 1000.0f );
	emit tweakableAdded( intensityTweakable );

	QIntTweakable* sigmaTweakable = new QIntTweakable( "sigma",
		[this](int p){ 
			buddhaMat->setSigma( ((float)p)/100.0f );
		},
		buddhaMat->sigma, 0.0, 10000.0f );
	emit tweakableAdded( sigmaTweakable );	
		
	QColorTweakable* buddhaColorTweakable = new QColorTweakable( "buddha Color",
		[this](vec4 color){ 
			buddhaMat->setDiffuseColor( color ); 
		},
		buddhaMat->diffuseColor );
	emit tweakableAdded( buddhaColorTweakable );

	initialized = true;
}

void BuddhaScene::deinit()
{
	if( initialized == true )
	{
		initialized = false;

		if( fboqt  != NULL ) delete fboqt;  fboqt  = NULL;
		if( fboqt2 != NULL ) delete fboqt2; fboqt2 = NULL;
		if( fboqt3 != NULL ) delete fboqt3; fboqt3 = NULL;
	}
}

void BuddhaScene::update(float dt)
{

}

void BuddhaScene::drawBuddha( bool depth )
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 100.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	buddha->Draw( depth ? DEPTH : COLOR );

	Renderer::get()->endFrame();
}

void BuddhaScene::computeDepthDifference()
{
	fboqt3->bind();

	glViewport(0,0,fbow, fboh);
	glClearColor(0.0,0.0, 0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	ShaderParams::get()->objectMatrix     = mat4(1.0);
	ShaderParams::get()->viewMatrix       = mat4(1.0);
	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.1f,100.0f);

	diffShader->enable( *ShaderParams::get() );
	diffShader->uTextureSampler1->setValue( fboqt2->texture() );
	diffShader->uTextureSampler2->setValue( fboqt->texture() );
	diffShader->uTextureMix->setValue( false );
	diffShader->uTextureMul->setValue( false );
	diffShader->uTextureDiff->setValue( true );

	diffShader->Draw( &(fullscreenQuad->mesh) );
	diffShader->disable();

	fboqt3->release();
	glViewport(0,0,ShaderParams::get()->win_x, ShaderParams::get()->win_y);
	glClearColor( 0.2f,0.2f,0.2f,1.0f );

}

void BuddhaScene::draw()
{
	fboqt->bind();
	glViewport( 0,0, fbow, fboh );
	glClearColor( 0.0f,0.0f,1.0f,1.0f );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	drawBuddha( true );
	fboqt->release();
	
	fboqt2->bind();
	glEnable( GL_CULL_FACE );
	glCullFace( GL_FRONT );
	glViewport( 0,0, fbow, fboh );
	glClearColor( 0.0f,0.0f,1.0f,1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	drawBuddha( true );
	fboqt2->release();

	// depth difference in the 3 framebuffer :
	computeDepthDifference();

	glViewport(0,0,ShaderParams::get()->win_x, ShaderParams::get()->win_y);
	glClearColor( 0.2f,0.2f,0.2f,1.0f );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	drawBuddha( false );

	Renderer::get()->beginFrame();

	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.1f,1000.0f);
	ShaderParams::get()->viewMatrix = mat4(1.0);

	glDisable( GL_CULL_FACE );
	colorImage->Draw(GUI);
	colorImage2->Draw(GUI);
	colorImage3->Draw(GUI);

	Renderer::get()->endFrame();
}