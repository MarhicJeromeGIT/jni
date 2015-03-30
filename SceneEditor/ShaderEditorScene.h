#ifndef SHADER_EDITOR_SCENE_H
#define SHADER_EDITOR_SCENE_H

#include <QtCore/QCoreApplication>
#include <qwidget.h>
#include <glm/glm.hpp>
#include "Scene.h"
#include <vector>
#include "Material.h"
#include <Shader.h>
#include <functional>

class OpenGLStaticModelInstance;
class MaterialGlass;
class QITweakable;
class Skybox;
class OpenGLSkinnedModelInstance;
class Image;
class DynamicMesh;
class GodrayShader;
class MaterialPhong;
class FramebufferObject;
class QOpenGLFramebufferObject;
class MaterialGrass;
class MaterialPhongTexture;
class CustomMaterial;
class OpenGLModelInstance;
class DynamicModelInstance;

enum UNIFORM_TYPE
{
	MODEL_MATRIX,
	VIEW_MATRIX,
	PROJECTION_MATRIX,
	NORMAL_MATRIX,
	INVVIEW_MATRIX,
	TIME,
	TEXTURE2D_0,
	TEXTURE2D_1,
	TEXTURE2D_2,
	TEXTURE2D_3,
	TEXTURE_CUBEMAP,
};

enum ATTRIBUTE_TYPE
{
	POSITION,
	TEXTURE_COORD,
	NORMAL_ATTRIBUTE,
	TANGENT_ATTRIBUTE,
};

enum EDITOR_MESH_TYPE
{
	EDITOR_SPHERE_MESH,
	EDITOR_PLANE_MESH,
	EDITOR_CUBE_MESH,
	EDITOR_CUSTOM_MESH,
};

struct CustomShaderParam
{
	TextureGL* tex0;
	TextureGL* tex1;
	TextureGL* tex2;
	TextureGL* tex3;

	TextureGL* cubemap;

	void setTexture( UNIFORM_TYPE type, TextureGL* tex );
};

class ShaderEditorScene : public Scene
{
	Q_OBJECT

public:
	OpenGLModelInstance*       currentMesh;
	OpenGLStaticModelInstance* sphereMesh;
	DynamicModelInstance*      planeMesh;
	OpenGLStaticModelInstance* cubeMesh;
	OpenGLStaticModelInstance* customMesh;

	CustomMaterial* customMat;

	void setMesh(EDITOR_MESH_TYPE meshType );
	Skybox* skybox;

	ShaderEditorScene(SceneManager* manager);

	void init();
	void deinit();
	void update(float dt);
	void draw();

	CustomShaderParam customParams;
	void updateTexture( map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap,  UNIFORM_TYPE type, TextureGL* tex );
};



class MyShader : public Shader
{
public:
	MyShader();
	bool ready; // compiled and correct

	bool compile(const std::string& vertexSource, const std::string& fragmentSource, map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap,
				map<std::string,ATTRIBUTE_TYPE>& aliasToAttributeTypeMap, CustomShaderParam& params);
	void updateParams(map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap, CustomShaderParam& params);

	vector<int> attribLoc;
	int vertexPositionAttrib;
	int textureCoordAttrib;
	int normalAttribLoc;
	int tangenteAttribLoc;

	vector< std::function< void() > > myUniformVector;

	void enable( const ShaderParams& params );
	void disable();
	void Draw(Mesh* mesh);

};

class CustomMaterial : public Material
{
public:
	MyShader* shader;
	CustomMaterial();

	bool compile(const std::string& vertexSource, const std::string& fragmentSource, map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap, 
				map<std::string,ATTRIBUTE_TYPE>& aliasToAttributeTypeMap, CustomShaderParam& params );
	const std::string& getErrorString();

	void updateParams(map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap, CustomShaderParam& params);

	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

#endif
