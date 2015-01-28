#include "Skybox.h"
#include "tga.h"
#include "ShaderList.h"
#include <glm/gtc/matrix_transform.hpp> 
#include "Renderer.h"
#include "Material.h"
#include "CommonLibrary.h"
#include "TextureGL.h"

using namespace glm;

// http://ogldev.atspace.co.uk/www/tutorial25/tutorial25.html

static const GLenum types[6] = {  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                                  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                                  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                                  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };


Skybox::Skybox( OpenGLModel* glModel, const std::string& data_path ) : OpenGLStaticModelInstance(mat4(1.0), glModel, glModel->internModel )
{
	LOGE("Skybox::Skybox")

	cubemap = TextureManager::get()->loadCubemapTexture( "skycubemap", data_path );

    material = new MaterialSkybox();
    material->setTexture( cubemap );
}

Skybox::Skybox( OpenGLModel* glModel, const std::string filename[6] ) : OpenGLStaticModelInstance(mat4(1.0), glModel, glModel->internModel )
{
	LOGE("Skybox::Skybox")

	cubemap = TextureManager::get()->loadCubemapTexture( "skycubemap", filename );

    material = new MaterialSkybox();
    material->setTexture( cubemap );
}

Skybox::~Skybox(void)
{
}

void Skybox::Draw(MATERIAL_DRAW_PASS Pass)
{
	if( material->getShader(Pass) != NULL )
	{
		DrawCall drawcall;
		drawcall.Pass = Pass;
		drawcall.modelMat = glm::scale( mat4(1.0), vec3(1000,1000,1000) );
		mat4 tempView = ShaderParams::get()->viewMatrix;
		tempView[3] = vec4(0.0);
		drawcall.viewMat = tempView;
		drawcall.projMat = ShaderParams::get()->projectionMatrix;
		drawcall.mesh    =  &(glModel->meshes[0]);
		drawcall.material = material;
		drawcall.hasTransparency = false;

		Renderer::get()->draw( drawcall );
	}
}
