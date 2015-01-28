#include "Image.h"
#include "Tweaker.h"
#include "TextureGL.h"
#include "CommonLibrary.h"

Image::Image( const vec3& pos, const std::string& filename ) : Overlay()
{
	position = pos;
	rotation = vec3(0,0,0);
	scale    = vec3(0.5,0.5,1.0);
	mesh     = NULL;
	overlayMaterial = new MaterialGUI();
	TextureGL* tex = TextureManager::get()->loadTexture( filename, filename );
	((MaterialGUI*)overlayMaterial)->setTexture( tex );


#define TWEAK_VALUE
#if defined(USE_TWEAKER) && defined(TWEAK_VALUE)
	Tweaker::get()->addTweakable( new FloatTweakable( "Image button X",
			[this](float f){ position = vec3(f,position.y,position.z); }, position.x, 0.0f,1.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "Image button Y",
			[this](float f){ position = vec3(position.x,f,position.z); }, position.y, 0.0f,1.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "Image Scale",
			[this](float f){ scale = vec3(f,f,1.0); }, scale.x, 0.0f,5.0f ));

#endif

	reloadGLData();
}

Image::Image( const vec3& pos, TextureGL* tex )
{
	position = pos;
	rotation = vec3(0,0,0);
	scale    = vec3(0.5,0.5,1.0);
	overlayMaterial = new MaterialGUI();
	((MaterialGUI*)overlayMaterial)->setTexture( tex );
	mesh = NULL;

#define TWEAK_VALUE
#if defined(USE_TWEAKER) && defined(TWEAK_VALUE)
	Tweaker::get()->addTweakable( new FloatTweakable( "Image button X",
			[this](float f){ position = vec3(f,position.y,position.z); }, position.x, 0.0f,1.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "Image button Y",
			[this](float f){ position = vec3(position.x,f,position.z); }, position.y, 0.0f,1.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "Image Scale",
			[this](float f){ scale = vec3(f,f,1.0); }, scale.x, 0.0f,5.0f ));

#endif

	reloadGLData();
}

void Image::reloadGLData()
{
	if( mesh != NULL )
		delete mesh;

	mesh = new DynamicMesh(4,2);
	vec3 vert[4] = { vec3(-0.5,-0.5,-10),vec3(0.5,-0.5,-10), vec3(0.5,0.5,-10), vec3(-0.5,0.5,-10) };
	vec2 uv[4]   = { vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,1) };
	unsigned short tri[2*3] = { 0,1,2, 0,2,3 };
	mesh->updateGeometry( vert, uv, 4, tri, 2  );
}

void Image::setUV( const vec2& bottomLeft, float w, float h )
{
	vec2 uv[4]   = { bottomLeft, bottomLeft + vec2(w,0), bottomLeft + vec2(w,h), bottomLeft + vec2(0,h) };
	mesh->updateBuffer( uv, 4,mesh->mesh.texCoordBuffer, false );
}

void Image::SetTextureCoordinates( const vec4& coords )
{
	vec2 bottomLeft = vec2( coords.z, coords.w );
	float w = coords.x;
	float h = coords.y;
	vec2 uv[4]   = { bottomLeft, bottomLeft + vec2(w,0), bottomLeft + vec2(w,h), bottomLeft + vec2(0,h) };
	mesh->updateBuffer( uv, 4,mesh->mesh.texCoordBuffer, false );

}

void Image::Draw(MATERIAL_DRAW_PASS Pass)
{
	mat4 objectmat =  glm::translate( mat4(1.0), position ) * glm::scale( mat4(1.0), scale );

	DrawCall drawcall;
	drawcall.Pass     = MATERIAL_DRAW_PASS::GUI;
	drawcall.modelMat = objectmat;
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	Shader* shader = overlayMaterial->getShader(MATERIAL_DRAW_PASS::GUI);
	if( shader != NULL )
	{
		drawcall.material = overlayMaterial;
		drawcall.mesh     = &(mesh->mesh);
		drawcall.transparencyMode = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
		drawcall.hasTransparency = true;
		Renderer::get()->draw( drawcall );
	}

}
