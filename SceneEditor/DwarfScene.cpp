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

#define BUILD_DATA 0

DwarfScene::DwarfScene(SceneManager* manager) : Scene(manager)
{
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
#if BUILD_DATA
	Assimp::Importer importer;
	ModelImport modelImport;

	const aiScene* scene2 = importer.ReadFile( DATA_PATH "dwarf_static.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	if( !scene2)
	{
		cout<<"error loading "<<"dwarf_static.dae"<<endl;
		cout<<importer.GetErrorString();
		assert(false);
		return;
	}
	bool result = modelImport.Import( scene2, modelDwarfAnim );
	assert( result == true );
	modelDwarfAnim->Save( DATA_PATH "dwarf_anim.txt" );

#endif

	modelDwarfAnim->Load( DATA_PATH "dwarf_anim.txt" );
	OpenGLStaticModel* dwarfAnimGL = new OpenGLStaticModel( modelDwarfAnim );
	mat4 dwarfMatrix = glm::translate( mat4(1.0), vec3(0,-1,0) ) * glm::rotate( mat4(1.0), -90.0f, vec3(1,0,0) );
	dwarfAnim = new OpenGLStaticModelInstance( dwarfMatrix, dwarfAnimGL );

	// Tweak the armor :
	MaterialBump* armorMaterial = new MaterialBump();
	TextureGL* armor_diff    = TextureManager::get()->loadTexture( "a_diff", "a_diff.png" );
	TextureGL* armor_ao      = TextureManager::get()->loadTexture( "a_ao", "a_ao.png" );
	TextureGL* armor_diff_ao = TextureManager::get()->CombinedTexture( "armor_diff_ao", TextureManager::TEXTURE_COMBINATION::MULTIPLY, armor_diff, armor_ao );
	TextureGL* armor_spec    = TextureManager::get()->loadTexture( "a_spec", "a_spec.png" );
	TextureGL* armor_norm    = TextureManager::get()->loadTexture( "a_norm", "a_norm.png" );
	
	armorMaterial->setColorMap( armor_diff );
	armorMaterial->setNormalMap( armor_norm );
	armorMaterial->setSpecularMap( armor_spec );
	armorMaterial->setColorMult(  vec4(0.160,0.095,0,1.0) );
	armorMaterial->setColorMult2( vec4(0.376,0.362,0.400,1.0) );
	dwarfAnim->setMaterial( "armor_material-material", armorMaterial );

	// Tweak the skin :
	MaterialBump* skinMaterial = new MaterialBump();
	TextureGL* skin_colorTex  = TextureManager::get()->loadTexture( "bb_diff", "b_diff.png" );
	TextureGL* tattoosTex     = TextureManager::get()->loadTexture( "btattoo", "paint_body.png" );
	TextureGL* combined       = TextureManager::get()->CombinedTexture( "bcombined", TextureManager::TEXTURE_COMBINATION::MIX, skin_colorTex, tattoosTex );
	TextureGL* skin_ao        = TextureManager::get()->loadTexture( "bb_ao", "b_ao.png" );
	TextureGL* skin_and_tatoo = TextureManager::get()->CombinedTexture( "bskin_and_tatoo", TextureManager::TEXTURE_COMBINATION::MULTIPLY, combined, skin_ao );
	TextureGL* skin_norm      = TextureManager::get()->loadTexture( "bb_norm", "b_normal.png" );
	TextureGL* skin_spec      = TextureManager::get()->loadTexture( "bb_spec", "b_spec.png" );

	skinMaterial->setColorMap( skin_colorTex );
	skinMaterial->setNormalMap( skin_norm );
	//skinMaterial->setSpecularMap( skin_spec );
	dwarfAnim->setMaterial( "body_material-material", skinMaterial );

	// Tweak the leather :
	TextureGL* leather_diff  = TextureManager::get()->loadTexture( "leather_diffuse", "leather_diffuse.png" );
	TextureGL* leather_norm  = TextureManager::get()->loadTexture( "leather_normal", "leather_normal.png" );
	TextureGL* leather_spec  = TextureManager::get()->loadTexture( "leather_specular", "leather_specular.png" );
	dwarfAnim->setMaterialDiffuseTexture( "leather_mat-material", leather_diff );
	dwarfAnim->setMaterialNormalTexture( "leather_mat-material", leather_norm );
	dwarfAnim->setMaterialSpecularTexture( "leather_mat-material", leather_spec );
	
	// no shadow :
	TextureGL* shadowMap = TextureManager::get()->createRGBATexture( "shadowmap_empty", vec4(1.0,1.0,1.0,1.0), 64, 64 );
	ShaderParams::get()->shadowmap = shadowMap->getTexId();

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

	dwarfAnim->Draw(COLOR);

	Renderer::get()->endFrame();
}


