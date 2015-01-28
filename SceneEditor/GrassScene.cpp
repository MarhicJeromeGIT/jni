#include "GrassScene.h"
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

const int nbCluster = 1000;
const int nbVertexInCluster = 12;
const int nbTrianglesInCluster = 6;

QOpenGLFramebufferObject* fboShadow;
QOpenGLFramebufferObject* fboShadowProj;

const int FBO_SHADOW_RES = 1024;

TextureGL* shadowTexture = NULL;

vector<glm::mat4> fenceMatrices;

GrassScene::GrassScene(SceneManager* manager) : Scene(manager)
{
	lightPosChanged = false;
	drawTextures    = false;
}

void GrassScene::init()
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

	Model* model     = new Model();
	Model* treeModel = new Model();
	Model* fenceModel = new Model();
#if BUILD_DATA
	Assimp::Importer importer;
	Assimp::Importer importer2;
	Assimp::Importer importer3;
	ModelImport modelImport, modelImport2, modelImport3;

	const aiScene* scene1 = importer.ReadFile( DATA_PATH "Forest_tree.dae", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );
	const aiScene* scene3 = importer2.ReadFile( DATA_PATH "fences.obj", aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices );

	if( !scene1 || !scene3 )
	{
		cout<<"error loading "<<"Beaubovin-lowpoly-animated-with-basic-rig.dae"<<endl;
		cout<<importer.GetErrorString();
		assert(false);
		return;
	}
	bool result = modelImport.Import( scene1, treeModel );
	assert( result == true );
	treeModel->Save( DATA_PATH "treeModel.txt" );


	result = modelImport3.Import( scene3, fenceModel );
	assert( result == true );
	fenceModel->Save( DATA_PATH "fenceModel.txt" );

#endif

	// tree model
	treeModel->Load( DATA_PATH "treeModel.txt" );
	OpenGLStaticModel* treeGL = new OpenGLStaticModel( treeModel );
	mat4 treeMatrix = glm::translate( mat4(1.0), vec3(0,0,0) ) * glm::rotate( mat4(1.0), -90.0f, vec3(1,0,0) );
	treeInstance = new OpenGLStaticModelInstance( treeMatrix, treeGL );

	// fence model
	fenceModel->Load( DATA_PATH "fenceModel.txt" );
	OpenGLStaticModel* fenceGL = new OpenGLStaticModel( fenceModel );
	mat4 fenceMatrix = glm::translate( mat4(1.0), vec3(-9,0,0) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 90.0f, vec3(0,1,0) );
	fenceInstance = new OpenGLStaticModelInstance( fenceMatrix, fenceGL );

	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(-9,0,6.0) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 90.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(-9,0,-6.0) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 90.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(-9,0,0) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 90.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(9,0,6.0) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 90.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(9,0,-6.0) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 90.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(9,0,0) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 90.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(6.5,0,-9) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 0.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(0,0,-9) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 0.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(-6.5,0,-9) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 0.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(6.5,0,9) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 0.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(0,0,9) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 0.0f, vec3(0,1,0) ) );
	fenceMatrices.push_back( glm::translate( mat4(1.0), vec3(-6.5,0,9) ) * glm::scale( mat4(1.0), vec3(2.0,2.0,2.0) ) * glm::rotate( mat4(1.0), 0.0f, vec3(0,1,0) ) );


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
	delete[] data;

	QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setMipmap(false);
    fboFormat.setSamples(0);
	fboFormat.setInternalTextureFormat(GL_RGBA);
	fboFormat.setAttachment( QOpenGLFramebufferObject::Attachment::Depth );

	fboShadow = new QOpenGLFramebufferObject( FBO_SHADOW_RES, FBO_SHADOW_RES, fboFormat );
	shadowTexture = new TextureGL( fboShadow->texture() );
	shadowImage = new Image( vec3( 0.15, 0.15, 0.0 ), shadowTexture );
	shadowImage->scale = vec3( 0.3, 0.3, 1.0 );

	fboShadowProj = new QOpenGLFramebufferObject( FBO_SHADOW_RES, FBO_SHADOW_RES, fboFormat );
	shadowProjImage = new Image( vec3( 0.15, 0.45, 0.0 ), new TextureGL( fboShadowProj->texture() ) );
	shadowProjImage->scale = vec3( 0.3, 0.3, 1.0 );

	glBindTexture( GL_TEXTURE_2D, fboShadow->texture() );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture( GL_TEXTURE_2D, fboShadowProj->texture() );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


	const float y = 0.0f;
	{
		plane = new DynamicMesh(4,2);
		const float S = 10.0f;
		vec3 vert[] = { vec3(-S,y,-S), vec3(S,y,-S), vec3(S,y,S), vec3(-S,y,S) };
		vec2 uv[]   = { vec2(0.0,0.0), vec2(10.0,0.0), vec2(10.0,10.0), vec2(0.0,10.0) };
		vec3 norm[] = { vec3(0,1,0), vec3(0,1,0), vec3(0,1,0), vec3(0,1,0) };
		unsigned short tri[2*3] = { 0,1,2, 0,2,3 };
		plane->updateGeometry( vert, uv, 4, tri, 2  );
		plane->updateBuffer( norm, 4, plane->mesh.normalBuffer );
	}

	{
		clusterPosition.clear();

		grassBlades = new DynamicMesh(4,2);
		vec3* vert = new vec3[ nbCluster * nbVertexInCluster ];
		vec2* uv   = new vec2[ nbCluster * nbVertexInCluster ];
		vec3* norm = new vec3[ nbCluster * nbVertexInCluster ];
		vec2* grassCoord = new vec2[ nbCluster * nbVertexInCluster ];

		unsigned short* tris = new unsigned short[ nbCluster * nbTrianglesInCluster * 3 ];
		
		for( int i=0; i< nbCluster; i++ )
		{
			vec2 offset = vec2 (srandf()*8.0f, srandf() * 8.0f );
			clusterPosition.push_back( vec3( offset.x, 0, offset.y ) );

			float randangle = srandf() * 2 * M_PI;
			float angles[] = { randangle + 0.0f, randangle + M_PI / 3.0f, randangle + 2.0f * M_PI / 3.0f };
			for( int blade = 0; blade < 3; blade++ )
			{
				vec2 xz  = offset + vec2( cos( angles[blade] ), sin( angles[blade] ) ) * 1.0f;
				vec2 xz2 = offset + vec2( cos( angles[blade] + M_PI ), sin( angles[blade] + M_PI ) ) * 1.0f;
				
				int baseIndex = nbVertexInCluster*i + blade * 4;
			
				vert[baseIndex  ] = vec3( xz.x,y,xz.y );
				vert[baseIndex+1] = vec3( xz2.x,y,xz2.y );
				vert[baseIndex+2] = vec3( xz2.x,y,xz2.y );
				vert[baseIndex+3] = vec3( xz.x,y,xz.y );

				uv[baseIndex  ] = vec2(0.0,0.0);
				uv[baseIndex+1] = vec2(1.0,0.0);
				uv[baseIndex+2] = vec2(1.0,0.1);
				uv[baseIndex+3] = vec2(0.0,0.1);

				grassCoord[baseIndex  ] = (xz  + vec2(8.0f, 8.0f)) / 16.0f;  
				grassCoord[baseIndex+1] = (xz2 + vec2(8.0f, 8.0f)) / 16.0f;  
				grassCoord[baseIndex+2] = (xz2 + vec2(8.0f, 8.0f)) / 16.0f;  
				grassCoord[baseIndex+3] = (xz  + vec2(8.0f, 8.0f)) / 16.0f;  

				norm[baseIndex  ] = vec3( 0,1,0 );
				norm[baseIndex+1] = vec3( 0,1,0 );
				norm[baseIndex+2] = vec3( 0,1,0 );
				norm[baseIndex+3] = vec3( 0,1,0 );

				int triIndex = ( nbTrianglesInCluster * i + blade * 2 ) * 3;

				tris[triIndex  ] = baseIndex;
				tris[triIndex+1] = baseIndex+1;
				tris[triIndex+2] = baseIndex+2;

				tris[triIndex+3] = baseIndex;
				tris[triIndex+4] = baseIndex+2;
				tris[triIndex+5] = baseIndex+3;

			}
		}
		grassBlades->updateGeometry( vert, uv, nbCluster * nbVertexInCluster, tris, nbCluster * nbTrianglesInCluster );
		grassBlades->updateBuffer( norm, nbCluster * nbVertexInCluster, grassBlades->mesh.normalBuffer );
		grassBlades->updateBuffer( grassCoord, nbCluster * nbVertexInCluster, grassBlades->mesh.texCoordBuffer2 );

		delete[] vert;
		delete[] uv;
		delete[] norm;
		delete[] tris;
		delete[] grassCoord;
	}

	phongMaterial = new MaterialPhongTexture();
	phongMaterial->setDiffuseColor( vec4(0.2,0.7,0.2,1.0 ) );
	TextureGL* grassTex = TextureManager::get()->loadTexture( "grass", "Grass.tga" );
	phongMaterial->setTexture( grassTex );

	TextureGL* grassBladeTex = TextureManager::get()->loadTexture( "grassblade", "grassblade.png" );
	grassMaterial = new MaterialGrass();
	grassMaterial->setTextureDiffuse( grassBladeTex );
	TextureGL* grassHeight = TextureManager::get()->loadTexture( "grassHeight", "grassHeight.png" );
	grassMaterial->setTextureHeight( grassHeight );
	TextureGL* grassFade = TextureManager::get()->loadTexture( "grassfade", "grassfade.png" );
	grassMaterial->setTextureFade( grassFade );
	grassMaterial->setTextureShadow( new TextureGL( fboShadowProj->texture() ) );
	grassMaterial->setIntensity( 5.0f );

	// Global lighting parameters
	ShaderParams::get()->ambient                       = vec4(0.5,0.5,0.5,1.0);
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(24,41,-50,1.0);
	ShaderParams::get()->lights[0].lightDiffuseColor   = glm::vec4(1.0,1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightSpecularColor  = glm::vec3(1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightShininess      = 32;

	// Tweakables
	QIntTweakable* lightAngleTweakable = new QIntTweakable( "light Angle",
		[this](int p){ 
			float distance = glm::length( vec3(24,41,-50) ); 
			ShaderParams::get()->lights[0].lightPosition.x = cos( p / 100.0f ) * distance; 
			ShaderParams::get()->lights[0].lightPosition.z = sin( p / 100.0f ) * distance; 
			lightPosChanged = true;
		},
		ShaderParams::get()->lights[0].lightPosition.x * 100.0f, -2*M_PI *100.0f, 2*M_PI *100.0f);
	emit tweakableAdded( lightAngleTweakable );

	QIntTweakable* lightPosTweakable = new QIntTweakable( "light pos Y",
		[this](int p){ 
			ShaderParams::get()->lights[0].lightPosition.y = ((float) p)/100.0f; 
			lightPosChanged = true;
		},
		ShaderParams::get()->lights[0].lightPosition.y * 100.0f, -200.0f * 100.0f, 200.0f * 100.0f );
	emit tweakableAdded( lightPosTweakable );	

	QIntTweakable* intensityTweakable = new QIntTweakable( "intensity",
		[this](int p){ 
			grassMaterial->setIntensity( ((float)p) / 100.0f );
		},
		5.0f * 100.0f, 0.0f * 100.0f, 10.0f * 100.0f );
	emit tweakableAdded( intensityTweakable );	

	QBooleanTweakable* drawTexturesTweakable = new QBooleanTweakable( "draw textures",
		[this](bool b){
			drawTextures = b;
		}, drawTextures );
	emit tweakableAdded( drawTexturesTweakable );

	shadowMapPass();
}

// generate the shadow from the tree from the light POV, then project it onto the ground plane.
void GrassScene::shadowMapPass()
{
	fboShadow->bind();
	glViewport( 0, 0, FBO_SHADOW_RES, FBO_SHADOW_RES );
	glClearColor( 1.0,1.0,1.0,1.0 );
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	ShaderParams::get()->projectionMatrix = glm::ortho(-10.0f,10.0f,-10.0f,10.0f, 0.1f, 100.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( vec3( ShaderParams::get()->lights[0].lightPosition ), vec3(0,0,0), vec3(0,1,0) );

	Renderer::get()->beginFrame();
	treeInstance->Draw( MATERIAL_DRAW_PASS::SHADOW );
		for( unsigned int i=0; i< fenceMatrices.size(); i++ )
	{
		fenceInstance->objectMatrix = fenceMatrices[i];
		fenceInstance->Draw(MATERIAL_DRAW_PASS::SHADOW);
	}

	Renderer::get()->endFrame();

	fboShadow->release();

	// now project it on the ground :

	// Light mat :
	mat4 LightMVP = ShaderParams::get()->projectionMatrix * ShaderParams::get()->viewMatrix;
	// bias
	mat4 matLight =mat4(	
		0.5, 0.0, 0.0, 0.0, 
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0 );
	LightMVP =  matLight * LightMVP;
	
	grassMaterial->setLightMap( shadowTexture );
	grassMaterial->setLightMat( LightMVP );

	fboShadowProj->bind();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	ShaderParams::get()->projectionMatrix = glm::ortho(-8.0f,8.0f,-8.0f,8.0f, 0.1f, 100.0f); // the ground is of size 10
	ShaderParams::get()->viewMatrix = glm::lookAt( vec3( 0.0, 5.0, 0.0 ), vec3(0,0,0), vec3(0,0,1) );

	Renderer::get()->beginFrame();
	DrawCall drawcall;

	mat4 objectMat = mat4(1.0);

	drawcall.Pass     = MATERIAL_DRAW_PASS::SHADOW;
	drawcall.modelMat = objectMat;
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	Shader* shader = grassMaterial->getShader(MATERIAL_DRAW_PASS::SHADOW);
	if( shader != NULL )
	{
		drawcall.material          = grassMaterial;
		drawcall.mesh              = &(plane->mesh);
		drawcall.transparencyMode  = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
		drawcall.hasTransparency   = false;
		drawcall.disableDepthWrite = false;
		Renderer::get()->draw( drawcall );
	}

	Renderer::get()->endFrame();

	fboShadowProj->release();

	glViewport( 0, 0, ShaderParams::get()->win_x, ShaderParams::get()->win_y );

}

void GrassScene::deinit()
{

}

void GrassScene::sort_indexes( vector<int>& indexes )
{
	// compute the distance to the camera :
	vector<float> distances( clusterPosition.size() );
	for( unsigned int i=0; i< clusterPosition.size(); i++ )
	{
		float length = glm::length( sceneManager->camera->position - clusterPosition[i] );
		distances[i] = length;
	}

	indexes.resize( clusterPosition.size() );
	for( unsigned int i=0; i< distances.size(); i++ )
	{
		indexes[i] = i;
	}

	// sort the indexes so the farthest blade is drawn first
	sort( indexes.begin(), indexes.end(), [&distances]( int i1, int i2 ){ return distances[i1] > distances[i2]; } );

	return;
}

void GrassScene::update(float dt)
{
	if( lightPosChanged )
	{
		lightPosChanged = false;
		shadowMapPass();
	}

	ShaderParams::get()->time += dt;

	// sort the grass cluster by distance to the camera :
	vector<int> indexes;
	sort_indexes( indexes );

	unsigned short tris[nbCluster * nbTrianglesInCluster * 3];
	int index = 0;
	for( unsigned int i=0; i< indexes.size(); i++ )
	{
		int idx = indexes[i];
		for( int blade = 0; blade < 3; blade++ )
		{
			int baseIndex = nbVertexInCluster*idx + blade * 4;
			
			tris[index]   = baseIndex;
			tris[index+1] = baseIndex+1;
			tris[index+2] = baseIndex+2;
			
			tris[index+3] = baseIndex;
			tris[index+4] = baseIndex +2;
			tris[index+5] = baseIndex +3;

			index += 6;
		}		
	}
	

	grassBlades->updateBuffer( tris, nbCluster * nbTrianglesInCluster * 3, grassBlades->mesh.indicesBuffer );
}

void GrassScene::drawGrass()
{
	DrawCall drawcall;

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
	}
}

void GrassScene::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	treeInstance->Draw(COLOR);
	for( unsigned int i=0; i< fenceMatrices.size(); i++ )
	{
		fenceInstance->objectMatrix = fenceMatrices[i];
		fenceInstance->Draw(COLOR);
	}
	drawGrass();

	DrawCall drawcall;

	mat4 objectMat = mat4(1.0);

	drawcall.Pass     = COLOR;
	drawcall.modelMat = objectMat;
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	Shader* shader = phongMaterial->getShader(COLOR);
	if( shader != NULL )
	{
		drawcall.material          = phongMaterial;
		drawcall.mesh              = &(plane->mesh);
		drawcall.transparencyMode  = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
		drawcall.hasTransparency   = false;
		drawcall.disableDepthWrite = false;
		Renderer::get()->draw( drawcall );
	}

	skybox->Draw(COLOR);

	Renderer::get()->endFrame();

	if( drawTextures )
	{
		Renderer::get()->beginFrame();
		ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.1f,1000.0f);
		ShaderParams::get()->viewMatrix = mat4(1.0);
		shadowImage->Draw(GUI);
		shadowProjImage->Draw(GUI);
		Renderer::get()->endFrame();
	}
}


