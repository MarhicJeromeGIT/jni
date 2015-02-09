#include "Material.h"

#include "DiffuseShader.h"
#include "TextureGL.h"
#include "Shader.h"
#include "GhostShader.h"

#include "MorphShader.h"
#include "DiscoBallShader.h"
#include "SkyboxShader.h"
#include "OceanShader.h"
#include "CubemapReflectionShader.h"
#include "CelShader.h"
#include "FullScreenEffectShader.h"
#include "SkinnedShader.h"
#include "ShockwaveShader.h"
#include "ExplosionWaveShader.h"
#include "TextShader.h"
#include "GlassShader.h"
#include "GrassShader.h"
#include "InstancingShader.h"
#include "DeferredShader.h"

using namespace glm;

Material::Material( MATERIAL_TYPE t ) : type(t)
{
}

const string& Material::getMaterialName()
{
	static const string matName[] =
	{
		"MATERIAL_SKINNING",
		"MATERIAL_MORPHING",
		"MATERIAL_OCEAN",
		"MATERIAL_CUBEMAP_REFLECTION",
		"MATERIAL_DIFFUSE",
		"MATERIAL_DIFFUSE_SHADOW",
		"MATERIAL_PHONG",
		"MATERIAL_PHONG_TEXTURE",
		"MATERIAL_CEL_SHADER",
		"MATERIAL_FIRE",
		"MATERIAL_SKYBOX",
		"MATERIAL_WATER",
		"MATERIAL_TEXTURE",
		"MATERIAL_MULTI_TEXTURE",
		"MATERIAL_TEXTURE_ARRAY",
		"MATERIAL_BUMP",
		"MATERIAL_DISCO",
		"MATERIAL_GHOST",
		"MATERIAL_FLAME_THROWER",
		"MATERIAL_LASER",
		"MATERIAL_PARTICLE",
		"MATERIAL_GUI",
		"MATERIAL_COLORED_TEXT",
	};

	return matName[type];
}

bool Material::CompareMaterial( Material* m1, Material* m2 )
{
	if( m1->type != m2->type )
	{
		return  m1->type < m2->type;
	}

	// same type of material, compare by texture :
	if( m1->type == MATERIAL_TYPE::MATERIAL_TEXTURE  )
	{
		MaterialTexture* t1 = (MaterialTexture*)m1;
		MaterialTexture* t2 = (MaterialTexture*)m2;

		// if the textures are different :
		//return t1->texId < t2->texId;
	}
/*	else if( m1->type == MATERIAL_TYPE::MATERIAL_GUI )
	{
		MaterialGUI* t1 = (MaterialGUI*)m1;
		MaterialGUI* t2 = (MaterialGUI*)m2;

		// if the textures are different :
		return t1->texId < t2->texId;

	}
*/

	return false;
}

//************************************//
// Texture Material
//************************************//

MaterialTexture::MaterialTexture() : Material( MATERIAL_TYPE::MATERIAL_TEXTURE )
{
	shader = (TextureShader*)ShaderManager::get()->getShader( SHADER_TYPE::TEXTURE_SHADER );
	shadowShader = NULL; // (ShadowMapGenerator*) ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
	tex = NULL;
	useScreenCoord = false;
}

void MaterialTexture::setTexture( TextureGL* t)
{
	tex = t;
}

void MaterialTexture::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		if( tex != NULL )
			shader->uTextureSampler->setValue( tex->getTexId() );

		shader->uUseScreenCoord->setValue(useScreenCoord);
	}
}

Shader* MaterialTexture::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH )
	{
		return NULL;// shadowShader;
	}
	else if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		return shader;
	}
	return NULL;
}

//************************************//
// Multi-Texture Material
//************************************//
#ifndef __ANDROID__
MaterialMultiTexture::MaterialMultiTexture(int nbTex ) : Material( MATERIAL_TYPE::MATERIAL_MULTI_TEXTURE )
{
	nbTextures = nbTex;
	shader = (MultiTextureShader*)ShaderManager::get()->getShader(SHADER_TYPE::MULTI_TEXTURE_SHADER);
//	shadowShader = (ShadowMapGenerator*) ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
	texId = new GLuint[nbTextures];
	for( int i=0; i< nbTextures; i++ )
		texId[i] = -1;
}

void MaterialMultiTexture::setTextureID( int textureIndex, GLuint id )
{
	assert(textureIndex < nbTextures );
	texId[textureIndex] = id;
}

void MaterialMultiTexture::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		for( int i=0; i< nbTextures; i++ )
		{
			if( texId[i] != -1 )
				shader->uTextureSampler[i]->setValue( texId[i] );
		}
	}
}

Shader* MaterialMultiTexture::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH )
	{
		return shadowShader;
	}
	else if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		return shader;
	}
	return NULL;
}
#endif

//************************************//
// GUI Material
//************************************//

MaterialGUI::MaterialGUI() : Material( MATERIAL_TYPE::MATERIAL_GUI )
{
	shader = (SimpleTextureShader*)ShaderManager::get()->getShader(SHADER_TYPE::SIMPLE_TEXTURE_SHADER);
	shaderTransparency = (PerVertexTransparencyTextureShader*)ShaderManager::get()->getShader(SHADER_TYPE::TEXTURE_SHADER_VERTEX_TRANSPARENCY);

	tex = NULL;
	UVScaleAndOffset = vec4(1.0,1.0,0.0,0.0);
	color = vec4(1,1,1,1);
	PerVertexTransparency = false;
}

void MaterialGUI::setUsePerVertexTransparency( bool transparency )
{
	PerVertexTransparency = transparency;
}

void MaterialGUI::setTexture(TextureGL* t )
{
	tex = t;
}

void MaterialGUI::setUVScaleAndOffset( const vec4& v )
{
	UVScaleAndOffset = v;
}

void MaterialGUI::setColor( const vec4& c)
{
	color = c;
}

void MaterialGUI::SetupUniforms(MATERIAL_DRAW_PASS Pass)
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		if( PerVertexTransparency )
		{
			if( tex != NULL )
				shaderTransparency->uTextureSampler->setValue( tex->getTexId() );
			shaderTransparency->uUVScaleAndOffset->setValue(UVScaleAndOffset);
		}
		else
		{
			if( tex != NULL )
				shader->uTextureSampler->setValue( tex->getTexId() );
			shader->uUVScaleAndOffset->setValue(UVScaleAndOffset);
			shader->color->setValue(color);
		}
	}
}

Shader* MaterialGUI::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::CONTOUR  )
	{
		return PerVertexTransparency ? shaderTransparency : shader;
	}
	return NULL;
}

//************************************//
// Texture Array Material
//************************************//
#ifndef __ANDROID__

MaterialTextureArray::MaterialTextureArray() : Material( MATERIAL_TYPE::MATERIAL_TEXTURE_ARRAY )
{
	shader = (TextureArrayShader*)ShaderManager::get()->getShader( SHADER_TYPE::TEXTURE_ARRAY_SHADER );
	tex = NULL;
	texId = -1;
	nbTextures = 1;
}

void MaterialTextureArray::setTextureID( GLuint id )
{
	texId = id;
}

void MaterialTextureArray::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	shader->uTextureSampler->setValue( texId );
}

Shader* MaterialTextureArray::getShader( MATERIAL_DRAW_PASS Pass )
{
	return shader;
}
#endif

//************************************//
// Diffuse Material
//************************************//

MaterialDiffuse::MaterialDiffuse() : Material( MATERIAL_TYPE::MATERIAL_DIFFUSE )
{
	shader =(DiffuseShader*)ShaderManager::get()->getShader( SHADER_TYPE::DIFFUSE_SHADER );
	diffuse_color = vec4(1.0,1.0,1.0,1.0);
//	shadowShader = (ShadowMapGenerator*) ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
}

void MaterialDiffuse::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	shader->uDiffuseColor->setValue( diffuse_color );
}

Shader* MaterialDiffuse::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH )
	{
		return shadowShader;
	}
	else
	{
		return shader;
	}
}

//************************************//
// Diffuse color with Shadows material
//************************************//

MaterialDiffuseShadow::MaterialDiffuseShadow() : Material( MATERIAL_TYPE::MATERIAL_DIFFUSE_SHADOW )
{
	shader = (DiffuseShadowShader*)ShaderManager::get()->getShader( SHADER_TYPE::DIFFUSE_SHADOW_SHADER );
	diffuse_color = vec4(1.0,1.0,1.0,1.0);
//	shadowShader = (ShadowMapGenerator*) ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
}

void MaterialDiffuseShadow::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	shader->uDiffuseColor->setValue( diffuse_color );
}

Shader* MaterialDiffuseShadow::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH )
	{
		return shadowShader;
	}
	else
	{
		return shader;
	}
}

//************************************//
// Phong Material
//************************************//

MaterialPhong::MaterialPhong() : Material( MATERIAL_TYPE::MATERIAL_PHONG )
{
	shader = (PhongShader*)ShaderManager::get()->getShader( SHADER_TYPE::PHONG_SHADER);
	shadowShader = (ShadowMapGenerator*) ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
	diffuseColor = vec4(1.0,1.0,1.0,1.0);
}

void MaterialPhong::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		shader->uDiffuseColor->setValue( diffuseColor );
	}
	else if( Pass == CONTOUR )
	{
		shadowShader->objectCol->setValue(1.0);
	}
}

void MaterialPhong::setDiffuseColor( const vec4& color )
{
	diffuseColor = color;
}

Shader* MaterialPhong::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH || Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		return shadowShader;
	}
	else
	{
		return shader;
	}
}


//************************************//
// Phong Texture Material
//************************************//

MaterialPhongTexture::MaterialPhongTexture() : Material( MATERIAL_TYPE::MATERIAL_PHONG_TEXTURE )
{
	shader = (PhongTextureShader*)ShaderManager::get()->getShader( SHADER_TYPE::PHONG_TEXTURE_SHADER);
	shadowShader = (ShadowMapGenerator*) ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
	diffuseColor = vec4(1.0,1.0,1.0,1.0);
	tex = NULL;
}

void MaterialPhongTexture::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		shader->uDiffuseColor->setValue( diffuseColor );
		if( tex != NULL )
			shader->uTextureSampler->setValue( tex->getTexId() );
	}
	else if( Pass == CONTOUR )
	{
		shadowShader->objectCol->setValue(1.0);
	}
}

void MaterialPhongTexture::setTexture( TextureGL* t )
{
	tex = t;
}

void MaterialPhongTexture::setDiffuseColor( const vec4& color )
{
	diffuseColor = color;
}

Shader* MaterialPhongTexture::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH || Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		return shadowShader;
	}
	else
	{
		return shader;
	}
}

//************************************//
// Phong With Depth Material
//************************************//

MaterialPhongWithDepth::MaterialPhongWithDepth() : Material( MATERIAL_TYPE::MATERIAL_PHONG_TEXTURE )
{
	shader = (PhongWithDepthShader*)ShaderManager::get()->getShader( SHADER_TYPE::PHONG_WITH_DEPTH);
	shadowShader = (ShadowMapGenerator*) ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
	diffuseColor = vec4(1.0,1.0,1.0,1.0);
	tex = NULL;
	intensity = 1.0f;
	sigma = 1.0f;
}

void MaterialPhongWithDepth::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		shader->uDiffuseColor->setValue( diffuseColor );
		if( tex != NULL )
			shader->uTextureSampler->setValue( tex->getTexId() );
		shader->uIntensity->setValue( intensity );
		shader->uSigma->setValue( sigma );
	}
	else if( Pass == CONTOUR )
	{
		shadowShader->objectCol->setValue(1.0);
	}
}

void MaterialPhongWithDepth::setTexture( TextureGL* t )
{
	tex = t;
}

void MaterialPhongWithDepth::setDiffuseColor( const vec4& color )
{
	diffuseColor = color;
}

Shader* MaterialPhongWithDepth::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH || Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		return shadowShader;
	}
	else
	{
		return shader;
	}
}

//************************************//
// Particle Material
//************************************//

MaterialParticle::MaterialParticle() : Material( MATERIAL_TYPE::MATERIAL_PARTICLE )
{
	shader = (ParticleShader*)ShaderManager::get()->getShader( SHADER_TYPE::PARTICLE_SHADER );
	texId = -1;
	scale = 1.0f;
}

void MaterialParticle::setTextureID( GLuint id )
{
	texId = id;
}

void MaterialParticle::setParticleSize(float scl)
{
	scale = scl;
}

void MaterialParticle::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( texId != -1 )
	{
		shader->textureUniform->setValue( texId );
	}
	shader->uParticleSize->setValue(scale);
}

Shader* MaterialParticle::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH )
	{
		return NULL;
	}
	else
	{
		return shader;
	}
}

//************************************//
// Fire Material
//************************************//

MaterialFire::MaterialFire() : Material( MATERIAL_TYPE::MATERIAL_FIRE )
{
	shader = (FireShader*)ShaderManager::get()->getShader( SHADER_TYPE::FIRE_SHADER );

	tex_flame   = TextureManager::get()->loadTexture("flame.tga", "flame.tga" );;
	tex_opacity = TextureManager::get()->loadTexture("flame_transparency.tga", "flame_transparency.tga" );
	tex_distort = TextureManager::get()->loadTexture("flame_distort.tga", "flame_distort.tga" );

	assert( tex_flame->error == false );
	assert( tex_opacity->error == false );
	assert( tex_distort->error == false );

	shader->uFlameSampler->setValue( tex_flame->getTexId() );
	shader->uOpacitySampler->setValue( tex_opacity->getTexId() );
	shader->uDistortionSampler->setValue( tex_distort->getTexId() );
}

void MaterialFire::setTextureID( GLuint id )
{
	texId = id;
}

void MaterialFire::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
/*	assert( tex_flame->error == false );
	assert( tex_opacity->error == false );
	assert( tex_distort->error == false );

	shader->uFlameSampler->setValue( tex_flame->getTexId() );
	shader->uOpacitySampler->setValue( tex_opacity->getTexId() );
	shader->uDistortionSampler->setValue( tex_flame->getTexId() );*/
}

Shader* MaterialFire::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH  )
	{
		return shader;
	}
	else
	{
		return shader;
	}
}

//************************************//
// Skinning Material
//************************************//

SkinningShaderMaterial::SkinningShaderMaterial() : Material( MATERIAL_TYPE::MATERIAL_SKINNING )
{
	shader           = (SkinningShader*)ShaderManager::get()->getShader( SHADER_TYPE::SKINNING_SHADER );
	shaderCube       = (SkinningShaderCubemapReflection*)ShaderManager::get()->getShader( SHADER_TYPE::SKINNING_CUBEMAP_SHADER );
	shaderBump       = (SkinningSpecularShader*)ShaderManager::get()->getShader( SHADER_TYPE::SKINNING_SPECULAR_SHADER );
	shaderBumpMix    = (SkinningSpecularMixDiffuseShader*)ShaderManager::get()->getShader( SHADER_TYPE::SKINNING_SPECULAR_DIFFUSE_MIX_SHADER );
	shadowShader     = (SkinningShadowMapShader*)ShaderManager::get()->getShader( SHADER_TYPE::SKINNING_SHADOW_SHADER );
	shaderBumpCrayon = (SkinningNormalCrayonShader*)ShaderManager::get()->getShader( SHADER_TYPE::SKINNING_SPECULAR_CRAYON_SHADER );

//	TextureGL* crayonTex = TextureManager::get()->loadTexture( "lines", DATA_PATH  "lines2.png" );
//	shaderBumpCrayon->uCrayonMap->setValue( crayonTex->getTexId() );

	colorMap         = NULL;
	normalMap    	 = NULL;
	specularMap  	 = NULL;
	cubeMap          = NULL;
	nbBones      	 = 0;
	bonesMatrix  	 = NULL;
	useDiffuseMix	 = false;
}

SkinningShaderMaterial::SkinningShaderMaterial( SkinningShaderMaterial* instance ) : Material( MATERIAL_TYPE::MATERIAL_SKINNING )
{
	shader          = instance->shader;
	shadowShader    = instance->shadowShader;
	shaderBump      = instance->shaderBump;
	colorMap        = instance->colorMap;
	normalMap       = instance->normalMap;
	specularMap     = instance->specularMap;
	nbBones         = instance->nbBones;
	bonesMatrix     = new mat4[ nbBones ];

	assert(nbBones > 0 );
	if( instance->bonesMatrix != NULL )
	{
		for( int i=0; i< nbBones; i++ )
		{
			bonesMatrix[i] = instance->bonesMatrix[i];
		}
	}
}

SkinningShaderMaterial::~SkinningShaderMaterial()
{
	if( bonesMatrix != NULL )
	{
		delete[] bonesMatrix;
	}
}

void SkinningShaderMaterial::setColorMult( const vec4& color )
{
	diffuseColor1 = color;
}

void SkinningShaderMaterial::setColorMult2( const vec4& color )
{
	useDiffuseMix = true;
	diffuseColor2 = color;
}

void SkinningShaderMaterial::setColorMap( TextureGL* t )
{
	colorMap = t;
}

void SkinningShaderMaterial::setNormalMap( TextureGL* tex )
{
	normalMap = tex;
}

void SkinningShaderMaterial::setCubemap( TextureGL* tex )
{
	cubeMap = tex;
}

void SkinningShaderMaterial::setInverseViewMat( const mat4& mat )
{
	InverseViewMat = mat;
}

void SkinningShaderMaterial::setSpecularMap( TextureGL* tex )
{
	specularMap = tex;
}

void SkinningShaderMaterial::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER)
	{
		if( normalMap == NULL )
		{
			if( cubeMap != NULL )
			{
				if( colorMap != NULL )
					shaderCube->uTexture->setValue( colorMap->getTexId() );

				if( shader->uBonesMat != NULL && bonesMatrix != NULL )
					glUniformMatrix4fv(shaderCube->uBonesMat->_location, nbBones , false, &(*bonesMatrix)[0][0] );

				shaderCube->uCubemap->setValue( cubeMap->getTexId() );
				shaderCube->uInverseViewMat->setValue( InverseViewMat );
			}
			else
			{
				if( colorMap != NULL )
					shader->uTexture->setValue( colorMap->getTexId() );

				if( shader->uBonesMat != NULL && bonesMatrix != NULL )
					glUniformMatrix4fv(shader->uBonesMat->_location, nbBones , false, &(*bonesMatrix)[0][0] );
			}
		}
		else
		{
			if( useDiffuseMix )
			{
				shaderBumpMix->uNormalMap->setValue( normalMap->getTexId() );

				if( specularMap != NULL )
					shaderBumpMix->uSpecularMap->setValue( specularMap->getTexId() );

				if( colorMap != NULL )
					shaderBumpMix->uTexture->setValue( colorMap->getTexId() );

				if( shader->uBonesMat != NULL && bonesMatrix != NULL )
					glUniformMatrix4fv(shaderBumpMix->uBonesMat->_location, nbBones , false, &(*bonesMatrix)[0][0] );

				shaderBumpMix->uColor1->setValue(diffuseColor1);
				shaderBumpMix->uColor2->setValue(diffuseColor2);
			}
			else
			{
				shaderBumpCrayon->uNormalMap->setValue( normalMap->getTexId() );

				if( specularMap != NULL )
					shaderBumpCrayon->uSpecularMap->setValue( specularMap->getTexId() );

				if( colorMap != NULL )
					shaderBumpCrayon->uTexture->setValue( colorMap->getTexId() );

				if( shader->uBonesMat != NULL && bonesMatrix != NULL )
					glUniformMatrix4fv(shaderBumpCrayon->uBonesMat->_location, nbBones , false, &(*bonesMatrix)[0][0] );
			}
		}
	}
	else if( Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		if( shadowShader->uBonesMat != NULL && bonesMatrix != NULL )
			glUniformMatrix4fv(shadowShader->uBonesMat->_location, nbBones , false, &(*bonesMatrix)[0][0] );

		shadowShader->objectCol->setValue(1.0);
	}

}

Shader* SkinningShaderMaterial::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		if(normalMap == NULL)
		{
			return cubeMap == NULL ? shader : shaderCube;
		}
		else
		{
			return (useDiffuseMix ? (SkinningSpecularShader*)shaderBumpMix : (SkinningSpecularShader*)shaderBumpCrayon);
		}
	}
	else if( Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		return shadowShader;
	}
	return NULL;
}

//************************************//
// Skybox Material
//************************************//

MaterialSkybox::MaterialSkybox() : Material( MATERIAL_TYPE::MATERIAL_SKYBOX )
{
	shader = (SkyboxShader*)ShaderManager::get()->getShader( SHADER_TYPE::SKYBOX_SHADER );
	tex  = NULL;
}

void MaterialSkybox::setTexture( TextureGL* t )
{
	tex = t;
}

void MaterialSkybox::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		if( tex != NULL )
			shader->uCubeMapSampler->setValue( tex->getTexId() );
	}
}

Shader* MaterialSkybox::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		return shader;
	}
	return NULL;
}

//************************************//
// Water Material
//************************************//

MaterialWater::MaterialWater() : Material( MATERIAL_TYPE::MATERIAL_WATER)
{
	shader = (WaterShader*)ShaderManager::get()->getShader( SHADER_TYPE::WATER_SHADER );
	shadowShader = (ShadowMapGenerator*) ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );

	TextureGL* tex = TextureManager::get()->loadTexture( "cubeclouds.tga", "cubeclouds.tga" );
	noiseId = tex->getTexId();
}

void MaterialWater::setTextureID( GLuint id )
{
	texId = id;
}

void MaterialWater::setReflectionMatrix( const mat4& mat )
{
	reflectionMat = mat;
}

void MaterialWater::SetupUniforms(MATERIAL_DRAW_PASS Pass)
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR )
	{
		shader->uWaterTextureSampler->setValue( texId );
		shader->uReflectedProjMat->setValue(  reflectionMat );
		if( shader->uNoiseSampler != NULL )
		{
			shader->uNoiseSampler->setValue( texId );
		}
	}
}

Shader* MaterialWater::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR )
	{
		return shader;
	}
	else if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH )
	{
		return shadowShader;
	}
	return NULL;
}


//************************************//
// Laser Material
//************************************//

MaterialLaser::MaterialLaser() : Material( MATERIAL_TYPE::MATERIAL_LASER )
{
	shader = (LaserShader*)ShaderManager::get()->getShader( SHADER_TYPE::LASER_SHADER);
	shaderSimple = (LaserShaderDummyVertex*)ShaderManager::get()->getShader( SHADER_TYPE::LASER_SIMPLE_SHADER);
}

void MaterialLaser::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
}

Shader* MaterialLaser::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::GUI )
	{
		return shaderSimple;
	}
	else if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		return shader;
	}
	return NULL;
}

//************************************//
// Flame Thrower Material
//************************************//

MaterialFlameThrower::MaterialFlameThrower() : Material( MATERIAL_TYPE::MATERIAL_FLAME_THROWER )
{
	shader = (FlameThrowerShader*)ShaderManager::get()->getShader( SHADER_TYPE::FLAMETHROW_SHADER);
	shaderGui = (FlameThrowerShaderSimple*)ShaderManager::get()->getShader( SHADER_TYPE::FLAMETHROW_SIMPLE_SHADER );
}

void MaterialFlameThrower::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
}

Shader* MaterialFlameThrower::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::GUI )
	{
		return shaderGui;
	}
	else if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		return shader;
	}
	return NULL;
}

//************************************//
// Bump Material
//************************************//

MaterialBump::MaterialBump() : Material( MATERIAL_TYPE::MATERIAL_BUMP )
{
	shader = (BumpShader*) ShaderManager::get()->getShader( SHADER_TYPE::BUMP_SHADER );
	shaderSpecularMap = (BumpSpecularMapShader*) ShaderManager::get()->getShader( SHADER_TYPE::BUMP_SPECULAR_MAP_SHADER );
	shaderMixSpecularMap = (BumpMixSpecularMapShader*) ShaderManager::get()->getShader( SHADER_TYPE::BUMP_MIX_SPECULAR_MAP_SHADER );
	shadowShader = (ShadowMapGenerator*) ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
	diffuseMult = vec4(1,1,1,1);
	diffuseMult2 = vec4(1,1,1,1);
	isMix = false;

	outline = false;
	colorMap = NULL;
	normalMap = NULL;
	specularMap = NULL;
	aoMap = TextureManager::get()->createRGBATexture("whiteTex", vec4(1.0,1.0,1.0,1.0), 16, 16 );
}

void MaterialBump::setColorMap( TextureGL* tex )
{
	colorMap = tex;
}

void MaterialBump::setColorMult( const vec4& color )
{
	diffuseMult = color;
}

void MaterialBump::setColorMult2( const vec4& color )
{
	diffuseMult2 = color;
	isMix = true;
//	assert(specularMap!= NULL);// we must have a specular map for this shader.
}

void MaterialBump::setNormalMap( TextureGL* tex )
{
	normalMap = tex;
}

void MaterialBump::setAOMap( TextureGL* tex )
{
	aoMap = tex;
}

void MaterialBump::setSpecularMap( TextureGL* tex )
{
	specularMap = tex;
}

void MaterialBump::setOutline( bool activate )
{
	outline = activate;
}

void MaterialBump::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		if( specularMap != NULL ) // we are using the specular map bump shader
		{
			if( isMix )
			{
				shaderMixSpecularMap->uSpecularMap->setValue( specularMap->getTexId() );
				if( colorMap != NULL )
					shaderMixSpecularMap->uTextureSampler->setValue( colorMap->getTexId() );
				if( normalMap != NULL )
					shaderMixSpecularMap->uNormalMap->setValue( normalMap->getTexId() );
				if( aoMap != NULL )
					shaderMixSpecularMap->uAOMap->setValue( aoMap->getTexId() );
				shaderMixSpecularMap->uOutline->setValue( outline );
				shaderMixSpecularMap->uDiffuseMult->setValue( diffuseMult );
				shaderMixSpecularMap->uDiffuseMult2->setValue( diffuseMult2 );
			}
			else
			{
				shaderSpecularMap->uSpecularMap->setValue( specularMap->getTexId() );
				if( colorMap != NULL )
					shaderSpecularMap->uTextureSampler->setValue( colorMap->getTexId() );
				if( normalMap != NULL )
					shaderSpecularMap->uNormalMap->setValue( normalMap->getTexId() );
				if( aoMap != NULL )
					shaderSpecularMap->uAOMap->setValue( aoMap->getTexId() );
				shaderSpecularMap->uOutline->setValue( outline );
				shaderSpecularMap->uDiffuseMult->setValue( diffuseMult );
			}
		}
		else
		{
			if( aoMap != NULL )
				shader->uAOMap->setValue( aoMap->getTexId() );
			if( colorMap != NULL )
				shader->uTextureSampler->setValue( colorMap->getTexId() );
			if( normalMap != NULL )
				shader->uNormalMap->setValue( normalMap->getTexId() );
			shader->uOutline->setValue( outline );
		}
	}

	if( Pass == CONTOUR )
	{
		shadowShader->objectCol->setValue(0.0);
	}
}

Shader* MaterialBump::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR  || Pass == MATERIAL_DRAW_PASS::WATER)
	{
		if( specularMap != NULL )
		{
			return isMix ? shaderMixSpecularMap : shaderSpecularMap;
		}
		else
		{
			return shader;
		}
	}
	else if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH || Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		return shadowShader;
	}
	return NULL;
}


//************************************//
// Ghost Material
//************************************//

MaterialGhost::MaterialGhost() : Material( MATERIAL_TYPE::MATERIAL_GHOST )
{
	shader = (GhostShader*)ShaderManager::get()->getShader( SHADER_TYPE::GHOST_SHADER );
	texId = -1;
}

void MaterialGhost::setTextureID( GLuint id )
{
	texId = id;
}

void MaterialGhost::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR )
	{
		if( texId != -1 )
			shader->uTextureSampler->setValue( texId );
	}
}

Shader* MaterialGhost::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR )
	{
		return shader;
	}
	return NULL;
}


//************************************//
// Morph Material
//************************************//

MaterialMorph::MaterialMorph() : Material( MATERIAL_TYPE::MATERIAL_MORPHING )
{
	shader =(MorphShader*)ShaderManager::get()->getShader( SHADER_TYPE::MORPH_SHADER );
	diffuse_color = vec4(1.0,1.0,1.0,1.0);
	morphCoeff = 0.5f;
	texID = 0;
}

void MaterialMorph::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	shader->uDiffuseColor->setValue( diffuse_color );
	shader->uMorphCoefficient->setValue( morphCoeff );
	shader->uTextureSampler->setValue( texID );
}

Shader* MaterialMorph::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR )
	{
		return shader;
	}
	return NULL;
}


//************************************//
// Diffuse Material
//************************************//

MaterialDisco::MaterialDisco() : Material( MATERIAL_TYPE::MATERIAL_DISCO )
{
	shader =(DiscoBallShader*)ShaderManager::get()->getShader( SHADER_TYPE::DISCO_BALL_SHADER );
	diffuse_color = vec4(1.0,1.0,1.0,1.0);
	texID  = 0;
}

void MaterialDisco::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	shader->uDiffuseColor->setValue( diffuse_color );
	shader->uTextureSampler->setValue( texID );
}

Shader* MaterialDisco::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR )
	{
		return shader;
	}
	return NULL;
}

//************************************//
// Colored Text Material
//************************************//

MaterialColoredText::MaterialColoredText() : Material( MATERIAL_TYPE::MATERIAL_COLORED_TEXT )
{
	shader = (ColorTextShader*)ShaderManager::get()->getShader(SHADER_TYPE::COLOR_TEXT_SHADER);
	texId = 0;
	UVScaleAndOffset = vec4(1.0,1.0,0.0,0.0);
}

void MaterialColoredText::setTextureID(GLuint id )
{
	texId = id;
}

void MaterialColoredText::setUVScaleAndOffset( const vec4& v )
{
	UVScaleAndOffset = v;
}

void MaterialColoredText::SetupUniforms(MATERIAL_DRAW_PASS Pass)
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR )
	{
		shader->uTextureSampler->setValue(texId);
		shader->uUVScaleAndOffset->setValue(UVScaleAndOffset);
	}
}

Shader* MaterialColoredText::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR  )
	{
		return shader;
	}
	return NULL;
}


//************************************//
// Ocean Material
//************************************//


MaterialOcean::MaterialOcean() : Material( MATERIAL_TYPE::MATERIAL_OCEAN )
{
	shader =(OceanShader*)ShaderManager::get()->getShader( SHADER_TYPE::OCEAN_SHADER );
	shadowShader = (ShadowMapGenerator*) ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
	diffuseColor = vec4(1.0,1.0,1.0,1.0);
	tex = NULL;
	cubemap = NULL;
	displacement = NULL;
	lambda = -1.0f;
}

void MaterialOcean::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::WATER )
	{
		shader->uDiffuseColor->setValue( diffuseColor );
		if( tex != NULL )
			shader->uTextureSampler->setValue( tex->getTexId() );

		if( cubemap != NULL )
			shader->uCubemapSampler->setValue( cubemap->getTexId() );

		if( displacement != NULL )
			shader->uHeightMapSampler->setValue( displacement->getTexId() );

		shader->lambda->setValue( lambda );
	}
	else if( Pass == CONTOUR )
	{
		shadowShader->objectCol->setValue(1.0);
	}
}

void MaterialOcean::setTexture( TextureGL* t )
{
	tex = t;
}

void MaterialOcean::setCubemap( TextureGL* t )
{
	cubemap = t;
}

void MaterialOcean::setDisplacementMap( TextureGL* tex )
{
	displacement = tex;
}

void MaterialOcean::setDiffuseColor( const vec4& color )
{
	diffuseColor = color;
}

Shader* MaterialOcean::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::SHADOW || Pass == MATERIAL_DRAW_PASS::DEPTH || Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		return shadowShader;
	}
	else
	{
		return shader;
	}
}

//************************************//
// Cubemap Reflection Material
//************************************//

MaterialCubemapReflection::MaterialCubemapReflection() : Material( MATERIAL_TYPE::MATERIAL_CUBEMAP_REFLECTION )
{
	shader        = (CubemapReflectionShader*)ShaderManager::get()->getShader( SHADER_TYPE::CUBEMAP_REFLECTION_SHADER );
	shaderTexture = (CubemapReflectionShaderWithTexture*)ShaderManager::get()->getShader( SHADER_TYPE::CUBEMAP_REFLECTION_TEXTURE_SHADER );
	shaderNormal  = (CubemapReflectionShaderWithTextureAndNormal*)ShaderManager::get()->getShader( SHADER_TYPE::CUBEMAP_REFLECTION_TEXTURE_NORMAL_SHADER );

	depthShader   = (ShadowMapGenerator*)ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
	cubemapTex    = NULL;
	diffuseTex    = NULL;
	normalTex     = NULL;
}

void MaterialCubemapReflection::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == COLOR )
	{
		if( diffuseTex != NULL )
		{
			if( normalTex != NULL )
			{
				if( cubemapTex != NULL )
					shaderNormal->uCubeMapSampler->setValue( cubemapTex->getTexId() );

				shaderNormal->uDiffuseSampler->setValue( diffuseTex->getTexId() );
				shaderNormal->uNormalMapSampler->setValue( normalTex->getTexId() );
			}
			else
			{
				if( cubemapTex != NULL )
					shaderTexture->uCubeMapSampler->setValue( cubemapTex->getTexId() );

				shaderTexture->uDiffuseSampler->setValue( diffuseTex->getTexId() );
			}
		}
		else
		{
			if( cubemapTex != NULL )
				shader->uCubeMapSampler->setValue( cubemapTex->getTexId() );
		}
	}
	else if( Pass == CONTOUR )
	{

		depthShader->objectCol->setValue(0.0);
	}
}

Shader* MaterialCubemapReflection::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR )
	{
		if( diffuseTex == NULL )
		{
			return shader;
		}
		else
		{
			if( normalTex == NULL )
			{
				return shaderTexture;
			}
			else
			{
				return shaderNormal;
			}
		}
	}
	else if( Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		return depthShader;
	}
	return NULL;
}


//************************************//
// Cel Shading with texture Material
//************************************//

MaterialCelShading::MaterialCelShading() : Material( MATERIAL_TYPE::MATERIAL_CEL_SHADER )
{
	shader = (CelShader*)ShaderManager::get()->getShader( SHADER_TYPE::CEL_SHADER);
	depthShader = (ShadowMapGenerator*)ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );

	diffuseColor        = vec4(1.0,1.0,1.0,1.0);
	tex                 = NULL;
	useScreenSpaceCoord = false;
}

void MaterialCelShading::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == CONTOUR )
	{
		depthShader->objectCol->setValue(1.0);
	}
	else
	{
		shader->uDiffuseColor->setValue( diffuseColor );
		shader->uUseScreenCoord->setValue( useScreenSpaceCoord );
		if( tex != NULL)
			shader->uTextureSampler->setValue( tex->getTexId() );
	}
}

void MaterialCelShading::setUseScreenSpaceCoord( bool screnSpaceCoord )
{
	useScreenSpaceCoord = screnSpaceCoord;
}

void MaterialCelShading::setTexture( TextureGL* t )
{
	tex = t;
}

void MaterialCelShading::setDiffuseColor( const vec4& color )
{
	diffuseColor = color;
}

Shader* MaterialCelShading::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		return depthShader;
	}
	return shader;
}


//************************************//
// Sobel Material
//************************************//

MaterialSobel::MaterialSobel() : Material( MATERIAL_TYPE::MATERIAL_SOBEL )
{
	shader = (SobelShader*)ShaderManager::get()->getShader( SHADER_TYPE::SOBEL_SHADER );
	tex = NULL;
	texWidth = 512.0f;
	texHeight = 512.0f;
}

void MaterialSobel::setTexture( TextureGL* t )
{
	tex = t;
}

void MaterialSobel::setTexDimension( const vec2& dim )
{
	texWidth  = dim.x;
	texHeight = dim.y;
}

void MaterialSobel::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( tex != NULL )
		shader->textureUniform->setValue( tex->getTexId() );

	shader->uTexHeight->setValue( texHeight );
	shader->uTexWidth->setValue( texWidth );
}

Shader* MaterialSobel::getShader( MATERIAL_DRAW_PASS Pass )
{
	return shader;
}


//************************************//
// Line Material
//************************************//

MaterialLine::MaterialLine() : Material( MATERIAL_TYPE::MATERIAL_LINE )
{
	shader = (LineShader*)ShaderManager::get()->getShader(SHADER_TYPE::LINE_SHADER);
	tex = NULL;
}

void MaterialLine::setTexture(TextureGL* t )
{
	tex = t;
}

void MaterialLine::SetupUniforms(MATERIAL_DRAW_PASS Pass)
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		if( tex != NULL )
			shader->uTextureSampler->setValue( tex->getTexId() );
	}
}

Shader* MaterialLine::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::CONTOUR  )
	{
		return shader;
	}
	return NULL;
}

//************************************//
// Shockwave Material
//
//************************************//

MaterialShockwave::MaterialShockwave() : Material( MATERIAL_TYPE::MATERIAL_SHOCKWAVE )
{
	shader = (ShockwaveShader*)ShaderManager::get()->getShader(SHADER_TYPE::SHOCKWAVE_SHADER);
	tex = NULL;
	time = 0.0f;
}

void MaterialShockwave::setTexture(TextureGL* t)
{
	tex = t;
}

void MaterialShockwave::SetupUniforms(MATERIAL_DRAW_PASS Pass)
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		if( tex != NULL )
		{
			shader->uTextureSampler->setValue( tex->getTexId() );
		}
		shader->uTime->setValue( time );
	}
}

Shader* MaterialShockwave::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::CONTOUR  )
	{
		return shader;
	}
	return NULL;
}

//************************************//
// Explosion wave Material
//
//************************************//

MaterialExplosionWave::MaterialExplosionWave() : Material( MATERIAL_TYPE::MATERIAL_SHOCKWAVE )
{
	shader = (ExplosionWaveShader*)ShaderManager::get()->getShader(SHADER_TYPE::EXPLOSION_WAVE_SHADER);
}

void MaterialExplosionWave::SetupUniforms(MATERIAL_DRAW_PASS Pass)
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
	}
}

Shader* MaterialExplosionWave::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::CONTOUR  )
	{
		return shader;
	}
	return NULL;
}


//************************************//
// Gaussian Blur Material
//
//************************************//

MaterialGaussianBlur::MaterialGaussianBlur() : Material( MATERIAL_TYPE::MATERIAL_LINE )
{
	shader = (GaussianBlurShader*)ShaderManager::get()->getShader(SHADER_TYPE::GAUSSIAN_BLUR_SHADER);
	isVerticalPass = false;
	tex = NULL;
}

void MaterialGaussianBlur::setTexture(TextureGL* t )
{
	tex = t;
}

void MaterialGaussianBlur::setIsVerticalPass( bool vertical )
{
	isVerticalPass = vertical;
}

void MaterialGaussianBlur::SetupUniforms(MATERIAL_DRAW_PASS Pass)
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		if( tex != NULL )
			shader->uTextureSampler->setValue( tex->getTexId() );

		shader->uVerticalPass->setValue(isVerticalPass);
	}
}

Shader* MaterialGaussianBlur::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::GUI || Pass == MATERIAL_DRAW_PASS::COLOR || Pass == MATERIAL_DRAW_PASS::CONTOUR  )
	{
		return shader;
	}
	return NULL;
}



//************************************//
// Glass Material
//
//************************************//

MaterialGlass::MaterialGlass() : Material( MATERIAL_TYPE::MATERIAL_GLASS )
{
	shader        = (GlassShader*)ShaderManager::get()->getShader( SHADER_TYPE::GLASS_SHADER );
	//shaderTexture = (CubemapReflectionShaderWithTexture*)ShaderManager::get()->getShader( SHADER_TYPE::CUBEMAP_REFLECTION_TEXTURE_SHADER );
	//shaderNormal  = (CubemapReflectionShaderWithTextureAndNormal*)ShaderManager::get()->getShader( SHADER_TYPE::CUBEMAP_REFLECTION_TEXTURE_NORMAL_SHADER );

	depthShader     = (ShadowMapGenerator*)ShaderManager::get()->getShader( SHADER_TYPE::SHADOW_MAP_GENERATOR_SHADER );
	cubemapTex      = NULL;
	diffuseTex      = NULL;
	normalTex       = NULL;
	invRefractIndex = 1.0f;
}

void MaterialGlass::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == COLOR )
	{
	/*	if( diffuseTex != NULL )
		{
			if( normalTex != NULL )
			{
				if( cubemapTex != NULL )
					shaderNormal->uCubeMapSampler->setValue( cubemapTex->getTexId() );

				shaderNormal->uDiffuseSampler->setValue( diffuseTex->getTexId() );
				shaderNormal->uNormalMapSampler->setValue( normalTex->getTexId() );
			}
			else
			{
				if( cubemapTex != NULL )
					shaderTexture->uCubeMapSampler->setValue( cubemapTex->getTexId() );

				shaderTexture->uDiffuseSampler->setValue( diffuseTex->getTexId() );
			}
		}
		else
	*/  {
			if( cubemapTex != NULL )
				shader->uCubeMapSampler->setValue( cubemapTex->getTexId() );

			shader->uInvRefractIndex->setValue( invRefractIndex );
		}
	}
	else if( Pass == CONTOUR )
	{

		depthShader->objectCol->setValue(0.0);
	}
}

Shader* MaterialGlass::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == MATERIAL_DRAW_PASS::COLOR )
	{
		return shader;
		/*
		if( diffuseTex == NULL )
		{
			return shader;
		}
		else
		{
			if( normalTex == NULL )
			{
				return shaderTexture;
			}
			else
			{
				return shaderNormal;
			}
		}
		*/
	}
	else if( Pass == MATERIAL_DRAW_PASS::CONTOUR )
	{
		return depthShader;
	}
	return NULL;
}

//************************************//
// Grass Material
//************************************//

MaterialGrass::MaterialGrass() : Material( MATERIAL_GRASS )
{
	shader = (MovingGrassShader*) ShaderManager::get()->getShader( SHADER_TYPE::GRASS_SHADER );
	shadowShader = (GrassShadowProjectionShader*) ShaderManager::get()->getShader( SHADER_TYPE::GRASS_SHADOW_SHADER );
	diffuseTex  = NULL;
	grassHeight = NULL;
	grassFade   = NULL;
	grassShadow = NULL;
	windDirection = vec2(1.0f,0.0f);
	intensity = 1.0f;

	lightMap = NULL;
	lightMat = glm::mat4(1.0);

}

void MaterialGrass::setTextureDiffuse( TextureGL* tex )
{
	diffuseTex = tex;
}

void MaterialGrass::setTextureHeight( TextureGL* tex )
{
	grassHeight = tex;
}

void MaterialGrass::setTextureFade( TextureGL* tex )
{
	grassFade = tex;
}

void MaterialGrass::setTextureShadow( TextureGL* tex )
{
	grassShadow = tex;
}

void MaterialGrass::setWindDirection( vec2& dir )
{
	windDirection = dir;
}

void MaterialGrass::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == COLOR )
	{
		if( diffuseTex != NULL )
		{
			shader->uTextureSampler->setValue( diffuseTex->getTexId() );
		}

		if( grassHeight != NULL )
		{
			shader->uGrassHeightSampler->setValue( grassHeight->getTexId() );
		}

		if( grassFade != NULL )
		{
			shader->uGrassFadeSampler->setValue( grassFade->getTexId() );
		}

		if( grassShadow != NULL )
		{
			shader->uGrassShadowSampler->setValue( grassShadow->getTexId() );
		}

		shader->uWindDirection->setValue( windDirection );
		shader->uIntensity->setValue( intensity );
	}
	else if( Pass == SHADOW )
	{
		if( lightMap != NULL )
		{
			shadowShader->lightMap->setValue( lightMap->getTexId() );
		}

		shadowShader->lightMat->setValue( lightMat );
		
	}
}

Shader* MaterialGrass::getShader( MATERIAL_DRAW_PASS Pass )
{
	if( Pass == SHADOW )
	{
		return shadowShader;
	}
	return shader;
}

//************************************//
// Static Instancing Material
//************************************//

MaterialInstanceStatic::MaterialInstanceStatic() : Material( MATERIAL_INSTANCE )
{
	shader = (InstancingShader*) ShaderManager::get()->getShader( SHADER_TYPE::INSTANCING_STATIC_SHADER );
}

void MaterialInstanceStatic::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	shader->uDiffuseColor->setValue( vec4(0.7,0.3,0.3,1.0) );
}

Shader* MaterialInstanceStatic::getShader( MATERIAL_DRAW_PASS Pass )
{
	return shader;
}

//************************************//
// Material3DSlice
//************************************//
#ifndef __ANDROID__

Material3DSlice::Material3DSlice() : Material( MATERIAL_TEXTURE )
{
	shader = (TextureSliceShader*) ShaderManager::get()->getShader( SHADER_TYPE::TEXTURE_3D_SLICE );
	tex    = NULL;
}

void Material3DSlice::setTexture( TextureGL* t)
{
	tex = t;
}

void Material3DSlice::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( tex != NULL )
		shader->uTextureSampler->setValue( tex->getTexId() );
}

Shader* Material3DSlice::getShader( MATERIAL_DRAW_PASS Pass )
{
	return shader;
}

#endif

//************************************//
//  Material Deferred
//************************************//

MaterialDeferred::MaterialDeferred() : Material( MATERIAL_DEFERRED )
{
	shader = (DeferredPassShader*) ShaderManager::get()->getShader( SHADER_TYPE::DEFERRED_SHADER );
	textureMap = NULL;
}

void MaterialDeferred::setTexture( TextureGL* map )
{
	textureMap = map;
}

void MaterialDeferred::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( textureMap != NULL )
		shader->uTextureSampler->setValue( textureMap->getTexId() );
}

Shader* MaterialDeferred::getShader( MATERIAL_DRAW_PASS Pass )
{
	return shader;
}

//************************************//
//  Material Deferred : Full screen shading
//************************************//

MaterialDeferredFullScreen::MaterialDeferredFullScreen() : Material( MATERIAL_DEFERRED_FULLSCREEN )
{
	shader = (DeferredShaderFullScreen*) ShaderManager::get()->getShader( SHADER_TYPE::DEFERRED_SHADER_FULLSCREEN );
	normalMap = NULL;
	vertexMap = NULL;
}

void MaterialDeferredFullScreen::setNormalMap( TextureGL* map )
{
	normalMap = map;
}

void MaterialDeferredFullScreen::setVertexMap( TextureGL* map )
{
	vertexMap = map;
}

void MaterialDeferredFullScreen::SetupUniforms( MATERIAL_DRAW_PASS Pass )
{
	if( normalMap )
		shader->uNormalMap->setValue(normalMap->getTexId());
	if( vertexMap )
		shader->uVertexMap->setValue(vertexMap->getTexId());
	shader->uLightColor->setValue( lightColor );
	shader->uLightPosition->setValue( lightPosition );
}

void MaterialDeferredFullScreen::setLightColor( glm::vec4 col )
{
	lightColor = col;
}

void MaterialDeferredFullScreen::setLightPosition( glm::vec4 pos )
{
	lightPosition = pos;
}

Shader* MaterialDeferredFullScreen::getShader( MATERIAL_DRAW_PASS Pass )
{
	return shader;
}

//***************
// MaterialLightCombine
//
//****************

MaterialLightCombine::MaterialLightCombine() : Material( MATERIAL_DEFERRED_LIGHT_COMBINE )
{
	shader = (DeferredShaderLightCombine*) ShaderManager::get()->getShader( DEFFERED_SHADER_LIGHT_COMBINE );
	colorMap = NULL;
	lightMap = NULL;
}

void MaterialLightCombine::setColorMap( TextureGL* map )
{
	colorMap = map;
}

void MaterialLightCombine::setLightMap( TextureGL* map )
{
	lightMap = map;
}

void MaterialLightCombine::SetupUniforms(MATERIAL_DRAW_PASS Pass)
{
	if( colorMap != NULL )
		shader->uColorMap->setValue( colorMap->getTexId() );
	if( lightMap != NULL )
		shader->uLightMap->setValue( lightMap->getTexId() );
}

Shader* MaterialLightCombine::getShader( MATERIAL_DRAW_PASS Pass )
{
	return shader;
}
