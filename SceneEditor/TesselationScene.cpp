#include "TesselationScene.h"
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
#include "TesselationShader.h"

#define terrain_resolution 64

//***************************************//
// Material for the terrain tesselation
//
//***************************************//
class MaterialTerrainTesselation : public Material
{
public:
	TerrainTesselationShader* shader;
	TextureGL* mainHeightmap;
	float intensity;

	MaterialTerrainTesselation() : Material( MATERIAL_TESSELATION )
	{
		shader = new TerrainTesselationShader();
		mainHeightmap = NULL;
		intensity = 5.0f;
	}

	void setHeightmapTexture( TextureGL* tex )
	{
		mainHeightmap = tex;
	}

	void setIntensity( float f )
	{
		intensity = f;
	}

	void SetupUniforms(MATERIAL_DRAW_PASS Pass)
	{
		shader->uDiffuseColor->setValue( vec4(0.9,0.4,0.4,1.0) );
		if( mainHeightmap != NULL )
		{
			shader->uHeightmap->setValue( mainHeightmap->getTexId() );
		}
		shader->uIntensity->setValue( intensity );
	}

	Shader* getShader( MATERIAL_DRAW_PASS Pass )
	{
		return shader;
	}
};

//*************************************//
// Tesselation Scene
//
//*************************************//
TesselationScene::TesselationScene(SceneManager* manager) : Scene(manager)
{
}

void TesselationScene::init()
{
	sceneManager->camera->position = glm::vec3(0,0,-10);

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

	glClearColor( 0.2, 0.2, 0.2, 1.0 );


	// Terrain material
	terrainMat = new MaterialTerrainTesselation();
	TextureGL* heightmap = TextureManager::get()->loadTexture( "heightmap.png", "heightmap.png" );
	terrainMat->setHeightmapTexture( heightmap );

	int nbVertices  = (1+terrain_resolution)*(1+terrain_resolution);
	int nbTriangles = terrain_resolution * terrain_resolution * 2;
	terrainMesh = new DynamicMesh( nbVertices, nbTriangles );

	vec3* vert = new vec3[ nbVertices ];
	vec2* uv   = new vec2[ nbVertices ];
	unsigned int* tris = new unsigned int[nbTriangles * 3];

	for( int i=0; i <= terrain_resolution; i++ )
	{
		for( int j=0; j<= terrain_resolution; j++ )
		{
			vert[ i* (1+terrain_resolution) + j ] = vec3( i - terrain_resolution/2,0,j-terrain_resolution/2 );

			uv[ i* (1+terrain_resolution) + j ] = vec2( ((float)i)/((float)terrain_resolution),((float)j)/((float)terrain_resolution) );
		}
	}
	int index = 0;
	for( int i=0; i < terrain_resolution; i++ )
	{
		for( int j=0; j< terrain_resolution; j++ )
		{
			tris[ index ]   = i*(1+terrain_resolution) + j;
			tris[ index +1] = i*(1+terrain_resolution) + j +1;
			tris[ index +2] = (i+1)*(1+terrain_resolution) + j +1;
		
			tris[ index +3] = i*(1+terrain_resolution) + j;
			tris[ index +4] = (i+1)*(1+terrain_resolution) + j +1;
			tris[ index +5] = (i+1)*(1+terrain_resolution) + j;

			index += 6;
		}
	}
	terrainMesh->updateGeometry( vert, uv, nbVertices, tris, nbTriangles );
	delete[] vert;
	delete[] uv;
	delete[] tris;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );

	QIntTweakable* intensityTweakable = new QIntTweakable( "intensity",
		[this](int p){ terrainMat->setIntensity( ((float)p)/100.0f); },
		terrainMat->intensity, 0.0f, 50.0f * 100.0f );
	emit tweakableAdded( intensityTweakable );
}

void TesselationScene::deinit()
{

}

void TesselationScene::update(float dt)
{
}

void TesselationScene::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	DrawCall drawcall;
	drawcall.modelMat = mat4(1.0);
	drawcall.Pass = COLOR;
	drawcall.material = terrainMat;
	drawcall.projMat = ShaderParams::get()->projectionMatrix;
	drawcall.viewMat = ShaderParams::get()->viewMatrix;
	drawcall.mesh    = &(terrainMesh->mesh);

	Renderer::get()->draw( drawcall );

	Renderer::get()->endFrame();
}


