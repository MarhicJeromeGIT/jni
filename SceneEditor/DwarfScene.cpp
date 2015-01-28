#include "DwarfScene.h"
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
#include "Gui\Image.h"

#define BUILD_DATA 1

DwarfScene::DwarfScene(SceneManager* manager) : Scene(manager)
{
	drawTextures = false;
}

void DwarfScene::init()
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

	Model* modelDwarfAnim = new Model();
	Model* skyboxModel = new Model();
	Model* castleModel = new Model();

#if BUILD_DATA
	Assimp::Importer importer;
	ModelImport modelImportDwarf,modelImportSky,modelImportCastle;
	
	// build the dwarf data
	const aiScene* sceneDwarf = importer.ReadFile( DATA_PATH "dwarf_static.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	assert(sceneDwarf);
	bool result = modelImportDwarf.Import( sceneDwarf, modelDwarfAnim );
	assert( result == true );
	modelDwarfAnim->Save( DATA_PATH "dwarf_anim.txt" );
	delete modelDwarfAnim;
	modelDwarfAnim = new Model();

	// build the castle
	const aiScene* sceneCastle = importer.ReadFile( DATA_PATH "castle.obj", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	assert(sceneCastle);
	result = modelImportCastle.Import( sceneCastle, castleModel );
	assert( result == true );
	castleModel->Save( DATA_PATH "castleModel.txt" );
	delete castleModel;
	castleModel = new Model();

	// build the sky
	const aiScene* sceneSky = importer.ReadFile( DATA_PATH "cube.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	result = modelImportSky.Import( sceneSky, skyboxModel );
	assert( result == true );
	skyboxModel->Save( DATA_PATH "skybox.txt" );
	delete skyboxModel;
	skyboxModel = new Model();

#endif

	// LOAD the skybox
	skyboxModel->Load( DATA_PATH "skybox.txt" );
	OpenGLStaticModel* skyboxGL = new OpenGLStaticModel(skyboxModel);
	std::string filename[6] = { DATA_PATH "skybox_texture_right.png",  DATA_PATH "skybox_texture_left.png",
							    DATA_PATH "skybox_texture_top.png",  DATA_PATH "skybox_texture_bottom.png",
								DATA_PATH "skybox_texture_front.png",  DATA_PATH "skybox_texture_back.png" };
	skybox  = new Skybox(skyboxGL, filename);

	// LOAD the castle
	castleModel->Load( DATA_PATH "castleModel.txt" );
	OpenGLStaticModel* castleGL = new OpenGLStaticModel( castleModel );
	mat4 castleMat = glm::scale( mat4(1.0), vec3(0.5,0.5,0.5) ) * glm::rotate( mat4(1.0), -90.0f, vec3(1,0,0) );
	castleInstance = new OpenGLStaticModelInstance( castleMat, castleGL );

	// LOAD the dwarf and setup the custom materials
	modelDwarfAnim->Load( DATA_PATH "dwarf_anim.txt" );
	OpenGLStaticModel* dwarfAnimGL = new OpenGLStaticModel( modelDwarfAnim );
	mat4 dwarfMatrix = glm::translate( mat4(1.0), vec3(0,0,0) ) * glm::rotate( mat4(1.0), -90.0f, vec3(1,0,0) );
	dwarfAnim = new OpenGLStaticModelInstance( dwarfMatrix, dwarfAnimGL );

	// Tweak the armor :
	armorMaterial = new MaterialBump();
	TextureGL* armor_diff    = TextureManager::get()->loadTexture( "a_diff.png", "a_diff.png" );
	TextureGL* armor_ao      = TextureManager::get()->loadTexture( "a_ao.png", "a_ao.png" );
	TextureGL* armor_spec    = TextureManager::get()->loadTexture( "a_spec.png", "a_spec.png" );
	TextureGL* armor_norm    = TextureManager::get()->loadTexture( "a_norm.png", "a_norm.png" );
	 
	armorMaterial->setColorMap( armor_diff );
	armorMaterial->setAOMap( armor_ao );
	armorMaterial->setNormalMap( armor_norm );
	armorMaterial->setSpecularMap( armor_spec );
	armorMaterial->setColorMult(  vec4(0.160,0.095,0,1.0) );
	armorMaterial->setColorMult2( vec4(0.376,0.362,0.400,1.0) );
	dwarfAnim->setMaterial( "armor_material-material", armorMaterial );

	armorTexture = new Image( vec3(0.20,0.75,-10), armor_ao );
	armorTexture->scale = vec3(0.30,0.30,1.0);
	
	// Tweak the skin :
	MaterialBump* skinMaterial = new MaterialBump();
	TextureGL* skin_colorTex  = TextureManager::get()->loadTexture( "b_diff.png", "b_diff.png" );
	TextureGL* tattoosTex     = TextureManager::get()->loadTexture( "paint_body.png", "paint_body.png" );
	// trouble is my friend.
	//TextureGL* skin_and_tatoo = TextureManager::get()->CombinedTexture( "skin_and_tatoo", TextureManager::TEXTURE_COMBINATION::MIX, skin_colorTex, tattoosTex );
	TextureGL* skin_ao        = TextureManager::get()->loadTexture( "b_ao.png", "b_ao.png" );
	TextureGL* skin_norm      = TextureManager::get()->loadTexture( "b_normal.png", "b_normal.png" );
	TextureGL* skin_spec      = TextureManager::get()->loadTexture( "b_spec.png", "b_spec.png" );

	skinMaterial->setColorMap( skin_colorTex );
	skinMaterial->setAOMap( skin_ao );
	skinMaterial->setNormalMap( skin_norm );
	skinMaterial->setSpecularMap( skin_spec );
	skinMaterial->setColorMult( vec4(1.0,1.0,1.0,1.0) );
	dwarfAnim->setMaterial( "body_material-material", skinMaterial );

	// Tweak the leather :
	TextureGL* leather_diff  = TextureManager::get()->loadTexture( "leather_diffuse.png", "leather_diffuse.png" );
	TextureGL* leather_norm  = TextureManager::get()->loadTexture( "leather_normal.png", "leather_normal.png" );
	TextureGL* leather_spec  = TextureManager::get()->loadTexture( "leather_specular.png", "leather_specular.png" );
	dwarfAnim->setMaterialDiffuseTexture( "leather_mat-material", leather_diff );
	dwarfAnim->setMaterialNormalTexture( "leather_mat-material", leather_norm );
	dwarfAnim->setMaterialSpecularTexture( "leather_mat-material", leather_spec );

	// no shadow :
	TextureGL* shadowMap = TextureManager::get()->createRGBATexture( "shadowmap_empty", vec4(1.0,1.0,1.0,1.0), 64, 64 );
	ShaderParams::get()->shadowmap = shadowMap->getTexId();

	// Global lighting parameters
	ShaderParams::get()->ambient                       = vec4(0.5,0.5,0.5,1.0);
	ShaderParams::get()->lights[0].lightDiffuseColor   = glm::vec4(1.0,1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightSpecularColor  = glm::vec3(1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightShininess      = 32;
	const float lightDistance = 10.0f;
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(lightDistance,15.0f,0.0f,1.0f);

	QIntTweakable* lightXTweakable = new QIntTweakable( "light angle",
		[lightDistance](int angle){ 
			ShaderParams::get()->lights[0].lightPosition.x = cos(((float) angle)*M_PI/180.0f) * lightDistance; 
			ShaderParams::get()->lights[0].lightPosition.z = sin(((float) angle)*M_PI/180.0f) * lightDistance; 
		},
		0 , 0, 365);
	emit tweakableAdded( lightXTweakable );

	QBooleanTweakable* textureTweakable = new QBooleanTweakable( "textures", [this](bool b){ drawTextures = b; }, drawTextures );
	emit tweakableAdded( textureTweakable );
	
	QColorTweakable* armorColor1Tweakable = new QColorTweakable( "Armor Color 1",
		[this](vec4 color){ 
			armorMaterial->setColorMult(  color );
		},
		ShaderParams::get()->lights[0].lightDiffuseColor );
	emit tweakableAdded( armorColor1Tweakable );

	QColorTweakable* armorColor2Tweakable = new QColorTweakable( "Armor Color 2",
		[this](vec4 color){ 
			armorMaterial->setColorMult2(  color );
		},
		ShaderParams::get()->lights[0].lightDiffuseColor );
	emit tweakableAdded( armorColor2Tweakable );

}

void DwarfScene::deinit()
{

}

void DwarfScene::update(float dt)
{

}

void DwarfScene::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	castleInstance->Draw(COLOR);
	dwarfAnim->Draw(COLOR);
	skybox->Draw( COLOR );

	Renderer::get()->endFrame();

	// 2D PASS
	if( drawTextures )
	{
		Renderer::get()->beginFrame();

		ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.1f,1000.0f);
		ShaderParams::get()->viewMatrix = mat4(1.0);

		armorTexture->Draw(GUI);

		Renderer::get()->endFrame();
	}
}


