#include "SceneManager.h"
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

#include "BuddhaScene.h"
#include "BuddhaGlass.h"
#include "DwarfScene.h"
#include "OceanScene.h"
#include "SkeletalAnimationScene.h"
#include "GodraysScene.h"
#include "GrassScene.h"
#include "InstancingScene.h"
#include "TesselationScene.h"
#include "PyroclasticNoiseScene.h"
#include "VikingScene.h"
#include "TTFont.h"
#include "DeferredRenderingScene.h"
#include "ShaderEditorScene.h"

SceneManager::SceneManager()
{
	isGLInit = false;
	camera = new Camera();
	camera_distance = 3.0f;

	for( int i=0; i< MY_KEY::MAX_KEYS; i++ )
	{
		keypress[i] = false;
	}

	typedef pair<string,Scene*> SceneItem;
		
	scenesItemArray.push_back( SceneItem( "Deferred Rendering",new DeferredScene( this ) ) );
	scenesItemArray.push_back( SceneItem( "Glass Refraction/Reflection",new BuddhaGlass( this ) ) );
	scenesItemArray.push_back( SceneItem( "Grass Scene",new GrassScene( this ) ) );
	scenesItemArray.push_back( SceneItem( "Dual Depth Peeling",new BuddhaScene( this ) ) );
	scenesItemArray.push_back( SceneItem( "Textured Dwarf",new DwarfScene( this ) ) );
	scenesItemArray.push_back( SceneItem( "Ocean",new OceanScene( this ) ) );
	scenesItemArray.push_back( SceneItem( "Skeletal Animation",new SkeletalAnimationScene( this ) ) );
	scenesItemArray.push_back( SceneItem( "Godrays",new GodrayScene( this ) ) );
	scenesItemArray.push_back( SceneItem( "Instancing",new InstancingScene( this ) ) );
	scenesItemArray.push_back( SceneItem( "Ray Tracing",new PyroclasticNoiseScene( this ) ) );
	scenesItemArray.push_back( SceneItem( "Movement Blur",new VikingScene( this ) ) );
	scenesItemArray.push_back( SceneItem( "ShaderEditorScene", new ShaderEditorScene( this) ) );
	currentScene = -1;
	ready = false;
}

void SceneManager::init()
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

		//select clearing (background) color
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// That's not very good.
	TextureManager::get()->setTexturePath(DATA_PATH);
	ShaderManager::get()->setShaderPath(SHADER_PATH);
	ShaderManager::get()->reloadShaders();
	TTFont::setFontPath("C:\\Users\\XiongBaoBao\\0101GravityBang\\Data\\Data\\");
	isGLInit = true;

	// If we already decided which scene to use, init it :
	if( currentScene != -1 )
	{
		SetAndInitCurrentScene( -1, currentScene );
	}
}

void SceneManager::deinit()
{
}

void SceneManager::update( float dt )
{
	if( ready )
	{
		scenesItemArray[currentScene].second->update( dt );
	}
}

void SceneManager::draw()
{
	if( ready )
	{
		scenesItemArray[currentScene].second->draw();
	}
}

void SceneManager::resizeGL( int width, int height )
{
	ShaderParams::get()->win_x = width;
	ShaderParams::get()->win_y = height;
}

void SceneManager::SetAndInitCurrentScene(int oldScene, int newScene)
{
	ready = false;
	if( oldScene >=0 && oldScene < scenesItemArray.size() ) 
	{
		scenesItemArray[oldScene].second->deinit();
	}
	
	scenesItemArray[newScene].second->init();
	currentScene = newScene;

	ready = true;		
}

void SceneManager::setScene( const std::string& sceneName )
{
	// look for the index of this new scene by comparing the name:
	for( unsigned int index = 0 ; index < scenesItemArray.size(); index++ )
	{
		if( sceneName == scenesItemArray[index].first )
		{
			if( currentScene != index ) // different scene
			{
				if( isGLInit )
				{
					SetAndInitCurrentScene(currentScene,index);
				}
				else //we will do it after we have inited opengl
				{
					currentScene = index;
				}
				break;
			}
		}
	}
}

Scene* SceneManager::getScene( const std::string& sceneName )
{
	// look for the index of this new scene by comparing the name:
	for( unsigned int index = 0 ; index < scenesItemArray.size(); index++ )
	{
		if( sceneName == scenesItemArray[index].first )
		{
			return scenesItemArray[index].second;
		}
	}
	return nullptr;
}

void SceneManager::mouseMoveEvent( int x, int y )
{	
	x -= 0.5*ShaderParams::get()->win_x;
	y -= 0.5*ShaderParams::get()->win_y;

	static float lastX = x;
	static float lastY = y;

	float dx = 2.0*(x - lastX)/ShaderParams::get()->win_x;
	float dy = 2.0*(y - lastY)/ShaderParams::get()->win_y;
	lastX = x;
	lastY = y;
	// rotate around the Y axis :
	static float phi = 0.0f;    // longitude
	static float theta = 0.0f;  // latitude (angle par rapport a l'equateur)

	float incY = dx * 2.0 * M_PI;
	phi += incY;

	float incX = dy * M_PI;
	theta += incX;
	theta = std::max( - M_2_PI, std::min( M_2_PI, (double) theta ));

//	if( keypress[MY_KEY::CONTROL] )
//	{
//		dwarf_angle += incY * 30.0f;
//		glm::mat4 dwarfmat = glm::translate( mat4(1.0), vec3(0.0,-1.0,0.0) ) * glm::rotate( glm::mat4(1.0), dwarf_angle, glm::vec3(0,1,0) ) * glm::rotate( mat4(1.0), -90.0f, vec3(1,0,0) );
//		dwarfAnim->objectMatrix = dwarfmat;
//	}
//	else
	{
		// http://fr.wikipedia.org/wiki/Coordonn%C3%A9es_sph%C3%A9riques
		float pos_x = camera_distance*cos(phi)*cos(theta);
		float pos_z = camera_distance*sin(phi)*cos(theta);
		float pos_y = camera_distance*sin(theta);

		camera->position = glm::vec3( pos_x, pos_y, pos_z );
	}
}

void SceneManager::mouseWheelEvent( int numSteps )
{
	camera_distance -= numSteps * 0.2f;

	float x = 0.5*ShaderParams::get()->win_x;
	float y = 0.5*ShaderParams::get()->win_y;

	mouseMoveEvent(x,y); // recompute the camera
}