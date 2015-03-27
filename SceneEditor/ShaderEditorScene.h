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

enum UNIFORM_TYPE
{
	MODEL_MATRIX,
	VIEW_MATRIX,
	PROJECTION_MATRIX,
	NORMAL_MATRIX,
};

class ShaderEditorScene : public Scene
{
	Q_OBJECT

public:

	OpenGLStaticModelInstance* mesh;
	CustomMaterial* customMat;

	Skybox* skybox;

	ShaderEditorScene(SceneManager* manager);

	void init();
	void deinit();
	void update(float dt);
	void draw();

	
	map<std::string,UNIFORM_TYPE> UniformTypeNames;
};

class MyShader : public Shader
{
public:
	MyShader();
	bool compile(const std::string& vertexSource, const std::string& fragmentSource, map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap);

	int vertexAttribLoc;

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

	bool compile(const std::string& vertexSource, const std::string& fragmentSource, map<std::string,UNIFORM_TYPE>& aliasToUniformTypeMap );
	const std::string& getErrorString();

	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

#endif
