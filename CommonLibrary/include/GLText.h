#ifndef GL_TEXT
#define GL_TEXT

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif
#include <stdlib.h>
#include <vector>
#include <string>
using namespace std;
#include <glm/glm.hpp>
#include "Material.h"
#include <stack>
#include "UTFTranslation.h"
#include <map>

class TextureGL;
class BitmapFont;
class DynamicMesh;

//********************************************************
// An opengl bitmap font
//
//********************************************************

enum FONTFAMILY
{
	DROID,
	FRANKY,
	CINNAMON,
	NANUM, // korean
	MAX_FONTS,
};

enum FONTFACES
{
	NORMAL,
	BOLD,
	ITALIC,
	MAX_FACES,
};


namespace MyNamespace
{
	class Font
	{
	public:
		float sizeY;
		float sizeX;

		Material* textMat;

		Font(){}
		virtual ~Font(){}
		virtual void reloadGLData(){};
		virtual void setFontColor( const glm::vec4& color ){ (void)color; }
		virtual void setDefaultFontFamily( FONTFAMILY family ){ (void)family; }
	public:
		virtual DynamicMesh* createGLString( const wstring& sentence ) = 0;
	};
};

class GLFont : public MyNamespace::Font
{
	friend class GLFontManager;
public:
	std::string fontName;

	vector<TextureGL*> pages;
	BitmapFont* font;
	GLuint tex2D;

	// en pixels :
	float size;
	float interline;
	glm::vec4 defaultColor;
	std::stack<glm::vec4> modifierStack;


private:
	GLFont( const std::string& fontName, const char* directory, const char* filename );

public:
	DynamicMesh* createGLString( const wstring& sentence );
};

//********************************************************
// A mesh holding a text string
//
//********************************************************


class DynamicText
{
public:
	DynamicMesh* lettersMesh;
	glm::vec3 rotation;
	glm::vec3 position;
	glm::vec3 scale;
	wstring sentence;

	MyNamespace::Font* glFont;

	float sizeY;
	float sizeX;

	float getSizeX(){ return sizeX; }
	float getSizeY(){ return sizeY; }

	DynamicText();
	DynamicText( const wstring& sentence );
	DynamicText(MyNamespace::Font* glFont );
	~DynamicText();
	void reloadGLData();


	void createGLString( const wstring& sentence );
	void Draw(MATERIAL_DRAW_PASS Pass);
};

#endif
