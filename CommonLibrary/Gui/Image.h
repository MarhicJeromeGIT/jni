#ifndef IMAGE_LAYOUT_H
#define IMAGE_LAYOUT_H

#include <string>
#include "Overlay.h"

class DynamicMesh;
#include "Material.h"

class Image : public Overlay
{
public:
	DynamicMesh* mesh;
	Material* overlayMaterial;
	vec3 rotation;
	vec3 position;
	vec3 scale;

	Image( const vec3& position, TextureGL* tex );
	Image( const vec3& position, const std::string& filename );
	virtual ~Image(){}
	virtual void reloadGLData();

	void Draw(MATERIAL_DRAW_PASS Pass);

	// deprecated
	void setUV( const vec2& bottomLeft, float w, float h );

	void SetTextureCoordinates( const vec4& coords );
};

#endif
