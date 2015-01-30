#ifndef _MATERIAL_H_
#define _MATERIAL_H_
/*
#ifdef __ANDROID__
	#include <stl/config/_android.h>
#endif
*/

#include <string>
using namespace std;

class Shader;
class TextureGL;
#include <glm/glm.hpp>

#include "MaterialDrawPass.h"

#include "../Shaders/DiffuseShader.h"
#include "../Shaders/PhongShader.h"
#include "../Shaders/TextureShader.h"
#include "../Shaders/ShadowMapGenerator.h"
#include "../Shaders/ParticleShader.h"
#include "../Shaders/FireShader.h"
#include "../Shaders/WaterShader.h"
#include "../Shaders/LaserShader.h"
#include "../Shaders/BumpShader.h"
#include "../Shaders/FlameThrowerShader.h"

class MorphShader;
class SkyboxShader;

#include "../Shaders/ShaderList.h"

class GhostShader;
class DiscoBallShader;
class OceanShader;
class CubemapReflectionShader;
class CubemapReflectionShaderWithTexture;
class CelShader;
class SobelShader;
class LineShader;
class SkinningShader;
class SkinningSpecularShader;
class SkinningShadowMapShader;
class SkinningSpecularMixDiffuseShader;
class SkinningNormalCrayonShader;
class ShockwaveShader;
class ExplosionWaveShader;
class GaussianBlurShader;
class PerVertexTransparencyTextureShader;
class ColorTextShader;
class SkinningShaderCubemapReflection;
class CubemapReflectionShaderWithTextureAndNormal;
class GlassShader;
class PhongWithDepthShader;
class MovingGrassShader;
class GrassShadowProjectionShader;
class InstancingShader;
class DeferredPassShader;
class DeferredShaderFullScreen;

#ifndef __ANDROID__ 
class TextureSliceShader;
#endif

enum MATERIAL_TYPE // un material - un shader
{
	MATERIAL_SKINNING,
	MATERIAL_MORPHING,
	MATERIAL_OCEAN,
	MATERIAL_CUBEMAP_REFLECTION,
	MATERIAL_GLASS,
	MATERIAL_DIFFUSE,
	MATERIAL_TESSELATION,
	MATERIAL_INSTANCE,
	MATERIAL_DIFFUSE_SHADOW,
	MATERIAL_PHONG,
	MATERIAL_PHONG_TEXTURE,
	MATERIAL_FIRE,
	MATERIAL_WATER,
	MATERIAL_GRASS,
	MATERIAL_TEXTURE,
	MATERIAL_CEL_SHADER,
	MATERIAL_MULTI_TEXTURE,
	MATERIAL_TEXTURE_ARRAY,
	MATERIAL_BUMP,
	MATERIAL_DISCO,
	MATERIAL_GHOST,
	MATERIAL_FLAME_THROWER,
	MATERIAL_LASER,
	MATERIAL_PARTICLE,
	MATERIAL_SHOCKWAVE,
	MATERIAL_LINE,
	MATERIAL_GUI,
	MATERIAL_COLORED_TEXT,
	MATERIAL_SOBEL,
	MATERIAL_SKYBOX,
	MATERIAL_DEFERRED,
	MATERIAL_DEFERRED_FULLSCREEN,
};

class Material
{
public:
	MATERIAL_TYPE type; // for sorting
	string name;

public:
	Material( MATERIAL_TYPE type);
	virtual ~Material(){}

	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass){ (void)Pass; };

	virtual Shader* getShader( MATERIAL_DRAW_PASS Pass ){ (void)Pass; return NULL; }

	static bool CompareMaterial( Material* m1, Material* m2 );

	const string& getMaterialName();
};

//************************************//
// Texture Material
//************************************//

class MaterialTexture : public Material
{
public:
	TextureShader* shader;
	TextureGL* tex;

	ShadowMapGenerator* shadowShader;

	bool useScreenCoord;
public:
	MaterialTexture();
	void setUseScreenSpaceCoord( bool use ){ useScreenCoord = use; }
	void setTexture( TextureGL* tex );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Multi-Texture Material
//************************************//

#ifndef __ANDROID__ 

class MaterialMultiTexture : public Material
{
public:
	MultiTextureShader* shader;
	int nbTextures;
	GLuint* texId;

	ShadowMapGenerator* shadowShader;
public:
	MaterialMultiTexture(int nbTextures);
	void setTextureID( int textureIndex, GLuint texId );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

#endif

//************************************//
// GUI Material
//************************************//

class MaterialGUI : public Material
{
public:
	SimpleTextureShader* shader;
	PerVertexTransparencyTextureShader* shaderTransparency;

	TextureGL* tex;
	glm::vec4 UVScaleAndOffset;
	glm::vec4 color;

	bool PerVertexTransparency;
public:
	MaterialGUI();
	void setUsePerVertexTransparency( bool transparency );
	void setTexture( TextureGL* tex );
	void setUVScaleAndOffset( const glm::vec4& UVScaleAndOffset );
	void setColor( const glm::vec4& color);
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Texture Array Material
//************************************//
#ifndef __ANDROID__ 

class MaterialTextureArray : public Material
{
public:
	TextureArrayShader* shader;
	TextureGL* tex;
	GLuint texId;
	int nbTextures;

public:
	MaterialTextureArray();
	void setTextureID( GLuint texId );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};
#endif

//************************************//
// Diffuse Material
//************************************//

class MaterialDiffuse : public Material
{
public:
	DiffuseShader* shader;
	glm::vec4 diffuse_color;

	ShadowMapGenerator* shadowShader;
public:
	MaterialDiffuse();
	void setDiffuse( const glm::vec4& color ){ diffuse_color = color; }
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};


//************************************//
// Diffuse color with Shadows material
//************************************//

class MaterialDiffuseShadow : public Material
{
public:
	DiffuseShadowShader* shader;
	glm::vec4 diffuse_color;

	ShadowMapGenerator* shadowShader;
public:
	MaterialDiffuseShadow();
	void setDiffuse( const glm::vec4& color ){ diffuse_color = color; }
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};


//************************************//
// Phong Material
//************************************//


class MaterialPhong : public Material
{
public:
	PhongShader* shader;
	ShadowMapGenerator* shadowShader;
	glm::vec4 diffuseColor;

public:
	MaterialPhong();

	void setDiffuseColor( const glm::vec4& color );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};


class MaterialPhongTexture : public Material
{
public:
	PhongTextureShader* shader;
	ShadowMapGenerator* shadowShader;
	glm::vec4 diffuseColor;
	TextureGL* tex;
public:
	MaterialPhongTexture();
	void setTexture( TextureGL* tex );
	void setDiffuseColor( const glm::vec4& color );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

class MaterialPhongWithDepth : public Material
{
public:
	PhongWithDepthShader* shader;
	ShadowMapGenerator* shadowShader;
	glm::vec4 diffuseColor;
	TextureGL* tex;
	float intensity;
	float sigma;
	glm::vec4 buddhaColor;

public:
	MaterialPhongWithDepth();
	void setIntensity( float i ){ intensity = i; }
	void setSigma( float s ){ sigma = s; }
	void setTexture( TextureGL* tex );
	void setDiffuseColor( const glm::vec4& color );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};
//************************************//
// Particle Material
//************************************//

class MaterialParticle : public Material
{
public:
	ParticleShader* shader;
	GLuint texId;
	float scale;
public:
	MaterialParticle();
	void setTextureID( GLuint texId );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);

	Shader* getShader( MATERIAL_DRAW_PASS Pass );

	void setParticleSize(float scale);
};


//************************************//
// Fire Material
//************************************//

class MaterialFire : public Material
{
public:
	FireShader* shader;
	TextureGL* tex_flame;
	TextureGL* tex_opacity;
	TextureGL* tex_distort;

	GLuint texId;

	ShadowMapGenerator* shadowShader;
public:
	MaterialFire();
	void setTextureID( GLuint texId );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Skinning Material
//************************************//

class SkinningShaderMaterial : public Material
{
	SkinningShader* shader;
	SkinningShaderCubemapReflection* shaderCube;
	SkinningSpecularShader* shaderBump;
	SkinningSpecularMixDiffuseShader* shaderBumpMix;
	SkinningNormalCrayonShader* shaderBumpCrayon;
	SkinningShadowMapShader* shadowShader;
public:
	glm::mat4* bonesMatrix;
	int nbBones;
	glm::mat4 InverseViewMat;

	TextureGL* colorMap;
	TextureGL* normalMap;
	TextureGL* specularMap;
	TextureGL* cubeMap;

	// for color mix
	bool useDiffuseMix;
	glm::vec4 diffuseColor1;
	glm::vec4 diffuseColor2;

public:
	SkinningShaderMaterial();
	SkinningShaderMaterial( SkinningShaderMaterial* instance );
	~SkinningShaderMaterial();

	void setColorMult( const glm::vec4& color );
	void setColorMult2( const glm::vec4& color );
	void setColorMap( TextureGL* tex );
	void setNormalMap( TextureGL* tex );
	void setCubemap( TextureGL* tex );
	void setInverseViewMat( const glm::mat4& mat );
	void setSpecularMap( TextureGL* tex );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Skybox Material
//************************************//

class MaterialSkybox : public Material
{
	SkyboxShader* shader;
	TextureGL* tex;
public:
	MaterialSkybox();
	void setTexture( TextureGL* tex);
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Water Material
//************************************//

class MaterialWater : public Material
{
	WaterShader* shader;
	GLuint texId;
	GLuint noiseId;
	glm::mat4 reflectionMat;

	ShadowMapGenerator* shadowShader;
public:
	MaterialWater();
	void setTextureID( GLuint texId );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );

	void setReflectionMatrix( const glm::mat4& reflectionMat );
};

//************************************//
// Laser Material
//************************************//

class MaterialLaser : public Material
{
public:
	LaserShader* shader;
	LaserShaderDummyVertex* shaderSimple;
public:
	MaterialLaser();
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Flame Thrower Material
//************************************//

class MaterialFlameThrower : public Material
{
public:
	FlameThrowerShader* shader;
	FlameThrowerShaderSimple* shaderGui;

public:
	MaterialFlameThrower();
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Bump Material
//************************************//

class MaterialBump : public Material
{
public:
	BumpShader* shader;
	BumpSpecularMapShader* shaderSpecularMap;
	BumpMixSpecularMapShader* shaderMixSpecularMap;

	TextureGL* colorMap;
	TextureGL* normalMap;
	TextureGL* specularMap;
	TextureGL* aoMap;

	glm::vec4 diffuseMult;
	bool isMix;
	glm::vec4 diffuseMult2;

	bool outline;

	ShadowMapGenerator* shadowShader;
public:
	MaterialBump();
	void setColorMap( TextureGL* tex );
	void setColorMult( const glm::vec4& color );
	void setColorMult2( const glm::vec4& color );
	void setNormalMap( TextureGL* tex );
	void setAOMap( TextureGL* tex );
	void setSpecularMap( TextureGL* tex );
	void setOutline( bool outline );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Ghost Material
//************************************//

class MaterialGhost : public Material
{
public:
	GhostShader* shader;
	GLuint texId;

public:
	MaterialGhost();
	void setTextureID( GLuint texId );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Morph Material
//************************************//

class MaterialMorph : public Material
{
public:
	MorphShader* shader;

	glm::vec4 diffuse_color;
	float morphCoeff;
	GLuint texID;

public:
	MaterialMorph();
	void setDiffuse( const glm::vec4& color ){ diffuse_color = color; }
	void setMorphCoeff( float coeff ){ morphCoeff = coeff; }
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	void setTextureID( GLuint id ){ texID = id; }

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Diffuse Material
//************************************//

class MaterialDisco : public Material
{
public:
	DiscoBallShader* shader;
	glm::vec4 diffuse_color;
	GLuint texID;

public:
	MaterialDisco();
	void setDiffuse( const glm::vec4& color ){ diffuse_color = color; }
	void setTextureID( GLuint id ){ texID = id; }
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};


//************************************//
// Colored Text Material
//************************************//

class MaterialColoredText : public Material
{
public:
	ColorTextShader* shader;
	GLuint texId;
	glm::vec4 UVScaleAndOffset;

public:
	MaterialColoredText();
	void setTextureID( GLuint texId );
	void setUVScaleAndOffset( const glm::vec4& UVScaleAndOffset );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Ocean Material
//************************************//

class MaterialOcean : public Material
{
public:
	OceanShader* shader;
	ShadowMapGenerator* shadowShader;
	glm::vec4 diffuseColor;
	float lambda;
	TextureGL* tex;
	TextureGL* cubemap;
	TextureGL* displacement;
public:
	MaterialOcean();
	void setTexture( TextureGL* tex );
	void setCubemap( TextureGL* tex );
	void setDisplacementMap( TextureGL* tex );
	void setDiffuseColor( const glm::vec4& color );
	void setLambda( float l ){ lambda = l; }
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Cubemap Reflection Material
//************************************//

class MaterialCubemapReflection : public Material
{
public:
	CubemapReflectionShader* shader;
	CubemapReflectionShaderWithTexture* shaderTexture;
	CubemapReflectionShaderWithTextureAndNormal* shaderNormal;
	ShadowMapGenerator* depthShader;

	TextureGL* cubemapTex;
	TextureGL* diffuseTex;
	TextureGL* normalTex;
public:
	MaterialCubemapReflection();
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	void setCubemapTex( TextureGL* tex ){ cubemapTex = tex; }
	void setTextureDiffuse( TextureGL* tex ){ diffuseTex = tex; }
	void setTextureNormal( TextureGL* tex ){ normalTex = tex; }

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};


//************************************//
// Cel shading with texture Material
//************************************//

class MaterialCelShading : public Material
{
public:
	CelShader* shader;
	ShadowMapGenerator* depthShader;

	bool useScreenSpaceCoord;
	glm::vec4 diffuseColor;
	TextureGL* tex;
public:
	MaterialCelShading();
	void setTexture( TextureGL* tex );
	void setUseScreenSpaceCoord( bool screenSpaceCoord );
	void setDiffuseColor( const glm::vec4& color );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Sobel Material
//************************************//

class MaterialSobel : public Material
{
public:
	SobelShader* shader;
	TextureGL* tex;
	float texWidth;
	float texHeight;
public:
	MaterialSobel();
	void setTexture( TextureGL* tex );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
	void setTexDimension( const glm::vec2& dim );
};

//************************************//
// Line Material
//************************************//

class MaterialLine : public Material
{
public:
	LineShader* shader;
	TextureGL* tex;

public:
	MaterialLine();
	void setTexture( TextureGL* tex );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Shockwave Material
//
//************************************//

class MaterialShockwave : public Material
{
public:
	ShockwaveShader* shader;
	TextureGL* tex;
	float time;
public:
	MaterialShockwave();
	void setTime( float t ){ time = t; }
	void setTexture( TextureGL* tex );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};


//************************************//
// Explosion wave Material
//
//************************************//

class MaterialExplosionWave : public Material
{
public:
	ExplosionWaveShader* shader;

public:
	MaterialExplosionWave();
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};


//************************************//
// Gaussian Blur Material
//************************************//

class MaterialGaussianBlur : public Material
{
public:
	GaussianBlurShader* shader;
	TextureGL* tex;
	bool isVerticalPass;

public:
	MaterialGaussianBlur();
	void setTexture( TextureGL* tex );
	void setIsVerticalPass( bool isVerticalPass );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};


//************************************//
// Glass Material
//************************************//

class MaterialGlass : public Material
{
public:
	GlassShader* shader;
	//CubemapReflectionShaderWithTexture* shaderTexture;
	//CubemapReflectionShaderWithTextureAndNormal* shaderNormal;
	ShadowMapGenerator* depthShader;

	TextureGL* cubemapTex;
	TextureGL* diffuseTex;
	TextureGL* normalTex;
	float invRefractIndex;
public:
	MaterialGlass();
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	void setCubemapTex( TextureGL* tex ){ cubemapTex = tex; }
	void setTextureDiffuse( TextureGL* tex ){ diffuseTex = tex; }
	void setTextureNormal( TextureGL* tex ){ normalTex = tex; }
	void setRefractIndex( float refract ){ invRefractIndex = 1.0f/refract; }

	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Grass Material
//************************************//

class MaterialGrass : public Material
{
public:
	TextureGL* diffuseTex;
	MovingGrassShader* shader;
	GrassShadowProjectionShader* shadowShader;

	glm::vec2 windDirection;
	TextureGL* grassHeight;
	TextureGL* grassFade; // the tone of green
	TextureGL* grassShadow; // shadow from the tree
	TextureGL* lightMap;
	glm::mat4  lightMat;
	float intensity;

	MaterialGrass();
	void setTextureDiffuse( TextureGL* tex );
	void setTextureHeight( TextureGL* grassHeight );
	void setTextureFade( TextureGL* grassFade );
	void setTextureShadow( TextureGL* grassShadow );
	void setLightMap( TextureGL* tex ){ lightMap = tex; }
	void setLightMat( glm::mat4& mat ){ lightMat = mat; }
	void setIntensity( float i ){ intensity = i; }
	void setWindDirection( glm::vec2& windDirection );
	virtual void SetupUniforms( MATERIAL_DRAW_PASS Pass );
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Static Instancing Material
//************************************//
class MaterialInstanceStatic : public Material
{
public:
	InstancingShader* shader;

	MaterialInstanceStatic();
	virtual void SetupUniforms( MATERIAL_DRAW_PASS Pass );
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Material3DSlice
//************************************//
#ifndef __ANDROID__ 

class Material3DSlice : public Material
{
public:
	TextureSliceShader* shader;

	TextureGL* tex;

public:
	Material3DSlice();
	void setTexture( TextureGL* tex );
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};
#endif

//************************************//
// Material Deferred
//************************************//

class MaterialDeferred : public Material
{
public:
	DeferredPassShader* shader;

public:
	MaterialDeferred();
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

//************************************//
// Material Deferred Fullscreen
//************************************//

class MaterialDeferredFullScreen : public Material
{
public:
	DeferredShaderFullScreen* shader;

	TextureGL* normalMap;
	TextureGL* vertexMap;
public:
	MaterialDeferredFullScreen();
	virtual void SetupUniforms(MATERIAL_DRAW_PASS Pass);
	void setNormalMap( TextureGL* map );
	void setVertexMap( TextureGL* map );
	Shader* getShader( MATERIAL_DRAW_PASS Pass );
};

#endif