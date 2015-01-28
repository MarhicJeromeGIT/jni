#pragma once

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>

	#include <assimp/mesh.h>
#endif

#include <string>
using namespace std;
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "OpenGLModel.h"

class Shader;
class OpenGLStaticModelInstance;
class OpenGLModel;

class Skybox : public OpenGLStaticModelInstance
{
public:
    Skybox( OpenGLModel* glModel, const std::string& data_path );
	Skybox( OpenGLModel* glModel, const std::string filename[6] );

    ~Skybox();

    void Bind(GLenum TextureUnit);
    void Draw(MATERIAL_DRAW_PASS Pass);

    MaterialSkybox* material;
public:

    TextureGL* cubemap;

};
