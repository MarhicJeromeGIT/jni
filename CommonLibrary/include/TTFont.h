#ifndef TTFONT_H
#define TTFONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include FT_CACHE_H
#include FT_CACHE_MANAGER_H

#include FT_GLYPH_H
#include FT_STROKER_H
#include FT_BITMAP_H
#include "BitmapFont.h"
#include "GLText.h"

class TTFont;
class FontManager
{
private:
	static FontManager* thisObject;
	FontManager();
	TTFont* ttfont;

public:
	static FontManager* get();
	MyNamespace::Font* getCurrentFont();
};

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif


GLuint TTFont_do();


struct FontModifier
{
	FontModifier()
	{
		color = glm::vec4(1,1,1,1);
		underlined = false;
		size = 1.0f;
		family = FONTFAMILY::DROID;
		face = FONTFACES::NORMAL;
	}

	glm::vec4 color;
	bool underlined;

	FONTFACES face;
	FONTFAMILY family;
	float size;
	// police ?
};

class TTFont : public MyNamespace::Font
{
private:
	FONTFAMILY currentFont;
	FONTFACES  currentFace;

	map<int,int> utfKey; // UTF-32 value to index in my vector
	vector<CharacterFont> characterMap;
	static const int texSize = 1024;
	unsigned char array[texSize][texSize][4];
	int currentX;
	int currentY;
	int maxRows;
	GLuint textureId;

	FT_Library library;
	std::stack<FontModifier> modifierStack;

	float interline;
public:

	static std::string FONT_PATH;
	static void setFontPath(const std::string& path);

	void setFontColor( const glm::vec4& color );
	void setDefaultFontFamily( FONTFAMILY family );

	void createFontFaces();
	TTFont();
	void reloadGLData();

	DynamicMesh* createGLString( const wstring& sentence );
};

#endif
