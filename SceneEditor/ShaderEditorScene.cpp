#include "ShaderEditorScene.h"
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
#include "macros.h"
#include "OpenGLModel.h"
#include <map>

ShaderEditorScene::ShaderEditorScene(SceneManager* manager) : Scene(manager)
{
	currentMesh = nullptr;
	sphereMesh  = nullptr;
	planeMesh   = nullptr;
	cubeMesh    = nullptr;
	customMesh  = nullptr;
}

void ShaderEditorScene::setMesh(EDITOR_MESH_TYPE meshType )
{
	switch( meshType )
	{
		case EDITOR_SPHERE_MESH :
			currentMesh = sphereMesh;
		break;
		case EDITOR_PLANE_MESH :
			currentMesh = planeMesh;
		break;
		case EDITOR_CUBE_MESH :
			currentMesh = cubeMesh;
		break;
		case EDITOR_CUSTOM_MESH :
			currentMesh = customMesh;
		break;
	}
}
void ShaderEditorScene::init()
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


	// Skybox
	Model* skyboxModel = new Model();
	skyboxModel->Load( DATA_PATH "skybox.txt" );
	OpenGLStaticModel* skyboxGL = new OpenGLStaticModel(skyboxModel);
	std::string filename[6] = { DATA_PATH "skybox_texture_right.png",  DATA_PATH "skybox_texture_left.png",
							    DATA_PATH "skybox_texture_top.png",  DATA_PATH "skybox_texture_bottom.png",
								DATA_PATH "skybox_texture_front.png",  DATA_PATH "skybox_texture_back.png" };
	skybox  = new Skybox(skyboxGL, filename);

	// Sphere mesh
	Model* sphereModel = new Model();
	sphereModel->Load( DATA_PATH "sphereModel.txt" );
	OpenGLStaticModel* sphereGL = new OpenGLStaticModel(sphereModel);
	sphereMesh = new OpenGLStaticModelInstance( glm::translate(mat4(1.0), vec3(0.0,0.5,0.0) ), sphereGL );

	customMat = new CustomMaterial();
	sphereMesh->setMaterial(0,customMat);
	

	currentMesh = sphereMesh;
	// Plane mesh :
	DynamicMesh* planeModel = new DynamicMesh(4,2);
	{	
		const float S = 2.0f;
		const float y = 0.0f;
		vec3 vert[] = { vec3(-S,y,-S), vec3(S,y,-S), vec3(S,y,S), vec3(-S,y,S) };
		vec2 uv[]   = { vec2(0.0,0.0), vec2(1.0,0.0), vec2(1.0,1.0), vec2(0.0,1.0) };
		vec3 norm[] = { vec3(0,1,0), vec3(0,1,0), vec3(0,1,0), vec3(0,1,0) };
		vec3 tan[] = { vec3(1,0,0), vec3(1,0,0), vec3(1,0,0), vec3(1,0,0) };
		vec3 bitan[] = { vec3(0,0,1), vec3(0,0,1), vec3(0,0,1), vec3(0,0,1) };
		unsigned short tri[2*3] = { 0,1,2, 0,2,3 };
		planeModel->updateGeometry( vert, uv, 4, tri, 2  );
		planeModel->updateBuffer( norm, 4, planeModel->mesh.normalBuffer );
		planeModel->updateBuffer( tan, 4, planeModel->mesh.tangenteBuffer );
		planeModel->updateBuffer( bitan, 4, planeModel->mesh.bitangenteBuffer );
	}
	planeMesh = new DynamicModelInstance( glm::mat4(1.0), planeModel );
	planeMesh->setMaterial(customMat);

	// Global lighting parameters
	ShaderParams::get()->ambient                       = vec4(0.5,0.5,0.5,1.0);
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(0,20,-5,1.0);
	ShaderParams::get()->lights[0].lightDiffuseColor   = glm::vec4(1.0,1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightSpecularColor  = glm::vec3(1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightShininess      = 32;
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(0,15,-50,1.0);
	ShaderParams::get()->time                          = 0.0f;


	customParams.tex0 = TextureManager::get()->createRGBATexture("red", glm::vec4(1,0,0,1), 16,16 );
	customParams.tex1 = TextureManager::get()->createRGBATexture("green", glm::vec4(0,1,0,1), 16,16 );
	customParams.tex2 = TextureManager::get()->createRGBATexture("blue", glm::vec4(0,0,1,1), 16,16 );
	customParams.tex3 = TextureManager::get()->createRGBATexture("black", glm::vec4(0,0,0,1), 16,16 );
	customParams.cubemap = skybox->cubemap;
}

void ShaderEditorScene::deinit()
{

}


void ShaderEditorScene::update(float dt)
{
	ShaderParams::get()->time += dt;
}


void ShaderEditorScene::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 1000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	currentMesh->Draw(COLOR);
	skybox->Draw(COLOR);

	Renderer::get()->endFrame();

}


//********************
// SHADER AND MATERIAL
//
//********************

MyShader::MyShader()
{
	ready = false;
	vertexPositionAttrib = -1;
	textureCoordAttrib   = -1;
	normalAttribLoc      = -1;
	tangenteAttribLoc    = -1;
}

// return true if no compilation error
bool MyShader::compile(const std::string& vertexSource, const std::string& fragmentSource, map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap,
						map<std::string,ATTRIBUTE_TYPE>& aliasToAttributeTypeMap, CustomShaderParam& params)
{
	bool compiled = newCompileProgram( vertexSource, fragmentSource );
	if( !compiled ) return false;
	ready = false;

	GetUniforms();

	for( auto it = _uniforms.begin(); it != _uniforms.end(); it++ )
	{
		cout<<it->first<<endl;
	}

	myUniformVector.clear();
	vertexPositionAttrib = -1;
	textureCoordAttrib   = -1;
	normalAttribLoc      = -1;
	tangenteAttribLoc    = -1;
	attribLoc.clear();

	auto setValueMat4f = [](Uniform* uniform, glm::mat4* matPtr)
	{
		((UniformMat4f*)uniform )->setValue( *matPtr );
	};
	auto setValueFloat = [](Uniform* uniform, float* floatPtr)
	{
		((UniformFloat*)uniform )->setValue( *floatPtr );
	};
	auto setValueNormalMat = [](Uniform* uniform, glm::mat4* viewMatrixPtr, glm::mat4* objectMatrixPtr)
	{
		glm::mat3 upperTopMV = glm::mat3( *viewMatrixPtr * *objectMatrixPtr );
		glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
		((UniformMat3f*)uniform )->setValue( normalMatrix );
	};
	auto setValueInvViewMat = [](Uniform* uniform, glm::mat4* viewMatrixPtr)
	{
		((UniformMat4f*)uniform )->setValue( glm::inverse( *viewMatrixPtr) );
	};
	auto setValueSampler2D = [](Uniform* uniform, TextureGL* tex)
	{
		((UniformSampler2D*)uniform )->setValue( tex->getTexId() );
	};
	auto setValueSamplerCube = [](Uniform* uniform, TextureGL* tex)
	{
		((UniformSamplerCube*)uniform )->setValue( tex->getTexId() );
	};
	glm::mat4* viewMatrix       = &( ShaderParams::get()->viewMatrix );
	glm::mat4* objectMatrix     = &( ShaderParams::get()->objectMatrix );
	glm::mat4* projectionMatrix = &( ShaderParams::get()->projectionMatrix );
	float* time                 = &( ShaderParams::get()->time );
	map<UNIFORM_TYPE, std::function< void (Uniform*) > > setValueMap;

	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::VIEW_MATRIX, std::bind( setValueMat4f, std::placeholders::_1, viewMatrix ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::MODEL_MATRIX, std::bind( setValueMat4f, std::placeholders::_1, objectMatrix ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::PROJECTION_MATRIX, std::bind( setValueMat4f, std::placeholders::_1, projectionMatrix ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::NORMAL_MATRIX, std::bind( setValueNormalMat, std::placeholders::_1, viewMatrix, objectMatrix ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::INVVIEW_MATRIX, std::bind( setValueInvViewMat, std::placeholders::_1, viewMatrix ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::TIME, std::bind( setValueFloat, std::placeholders::_1, time ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::TEXTURE2D_0, std::bind( setValueSampler2D, std::placeholders::_1, params.tex0 ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::TEXTURE2D_1, std::bind( setValueSampler2D, std::placeholders::_1, params.tex1 ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::TEXTURE2D_2, std::bind( setValueSampler2D, std::placeholders::_1, params.tex2 ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::TEXTURE2D_3, std::bind( setValueSampler2D, std::placeholders::_1, params.tex3 ) ) );
	setValueMap.insert( pair< UNIFORM_TYPE, std::function< void (Uniform*) > >(UNIFORM_TYPE::TEXTURE_CUBEMAP, std::bind( setValueSamplerCube, std::placeholders::_1, params.cubemap ) ) );

	for( auto it = aliasToUniformTypeMap.begin(); it != aliasToUniformTypeMap.end(); it++ )
	{
		Uniform* myUniform = NULL;
		myUniform = GetUniformByName( it->first );
		if( myUniform != NULL )
		{
			auto myFunc = std::bind( setValueMap[it->second], myUniform );
			myUniformVector.push_back( myFunc );
		}
	}	
	
	for( auto it = aliasToAttributeTypeMap.begin(); it != aliasToAttributeTypeMap.end(); it++ )
	{
		int myAttrib = glGetAttribLocation( getProgram(), it->first.c_str() );
	
		if( myAttrib >= 0 )
		{
			attribLoc.push_back( myAttrib );

			switch( it->second )
			{
			case POSITION:
				vertexPositionAttrib = myAttrib;
				break;
			case TEXTURE_COORD:
				textureCoordAttrib = myAttrib;
				break;
			case NORMAL_ATTRIBUTE:
				normalAttribLoc = myAttrib;
				break;
			case TANGENT_ATTRIBUTE:
				tangenteAttribLoc = myAttrib;
				break;
			default:
				assert(false);
				break;
			};
		}
	}

	ready = true;
	return compiled;
}

void MyShader::Draw(Mesh* m)
{
	if( !ready )  return;

	InternalMesh* mesh = (InternalMesh*) m;
	Clean();

	if( vertexPositionAttrib >= 0 )
	{
		glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
		glVertexAttribPointer( vertexPositionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if( textureCoordAttrib >= 0 )
	{
		glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordBuffer );
		glVertexAttribPointer( textureCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if( normalAttribLoc >= 0 )
	{
		glBindBuffer( GL_ARRAY_BUFFER, mesh->normalBuffer );
		glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if( tangenteAttribLoc >= 0 )
	{
		glBindBuffer( GL_ARRAY_BUFFER, mesh->tangenteBuffer );
		glVertexAttribPointer( tangenteAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
	glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
}

void MyShader::enable( const ShaderParams& params )
{
	if( !ready ) return;
	
	glUseProgram(_program);

	for( auto& it : attribLoc )
	{
			glEnableVertexAttribArray( it );
	}

	for( auto& it : myUniformVector )
	{
		it();
	}
}

void MyShader::disable()
{
	if( !ready ) return;
	for( auto& it : attribLoc )
	{
		glDisableVertexAttribArray( it );
	}

	Shader::disable();
}

CustomMaterial::CustomMaterial() : Material(MATERIAL_SKINNING)
{
	shader = new MyShader();

} 

bool CustomMaterial::compile(const std::string& vertexSource, const std::string& fragmentSource,
							 map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap, map<std::string,ATTRIBUTE_TYPE>& aliasToAttributeTypeMap,
							 CustomShaderParam& params)
{
	return shader->compile( vertexSource, fragmentSource, aliasToUniformTypeMap, aliasToAttributeTypeMap, params );
}

const std::string& CustomMaterial::getErrorString()
{
	return shader->getErrorString();
}

void CustomMaterial::SetupUniforms(MATERIAL_DRAW_PASS Pass)
{
	//shader->uColorMap->setValue( colorMap->getTexId() );
}

Shader* CustomMaterial::getShader( MATERIAL_DRAW_PASS Pass )
{
	return shader;
}