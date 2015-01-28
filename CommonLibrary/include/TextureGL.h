#pragma once

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <string>
using namespace std;
#include <vector>

class TextureGL;

//*******************************
// Texture Manager
//
//*******************************

class TextureManager
{
private:
	TextureManager();
	static TextureManager* thisObject;

	vector<TextureGL*> textures;

	string TEXTURE_PATH;
public:
	static TextureManager* get();

	void setTexturePath(const std::string& path);

	TextureGL* loadTexture(  const std::string&  name,  const std::string&  filename );
	void deleteTexture( TextureGL* tex );

	TextureGL* getTexture( const string& name );

	TextureGL* createRGBATexture( const std::string&  name, const glm::vec4& color, int sizeX, int sizeY );
	TextureGL* createDepthTexture( const std::string& name, int sizeX, int sizeY );
	TextureGL* loadCubemapTexture( const std::string& name, const std::string data_path );
	TextureGL* loadCubemapTexture( const std::string& name, const std::string filename[6] );

	enum TEXTURE_COMBINATION
	{
		MIX,
		MULTIPLY,
	};
	TextureGL* CombinedTexture( const std::string& name, TEXTURE_COMBINATION combine, TextureGL* tex1, TextureGL* tex2 );

	void reloadTextures();

};


//*******************************
// TextureGL
//
//*******************************

class TextureGL
{
public: 
	// meme pas peur !
	TextureGL( int id ){ texId = id; };

protected:
	friend class TextureManager; // only the texture manager can load textures.
	TextureGL();
	~TextureGL();

	void CreateFileTexture( const std::string& name, const std::string& filename );
	void CreateDepthTexture( const std::string& name, int sizeX, int sizeY );
	void CreateRGBATexture( const std::string&  name, const glm::vec4& color, int sizeX, int sizeY );
	virtual void Load();

protected:
	enum TEXTURE_TYPE
	{
		UNDEFINED,
		TEXTURE_FILE, // loaded from disk
		TEXTURE_COLOR, // made by programming
		DEPTH,
		CUBEMAP,
	};
	TEXTURE_TYPE textureType;

protected:
	bool hasTransparency;
	GLuint texId;
	int width; 
	int height;
	int bpp;
	glm::vec4 color;

public:
	string name;
	string filename; // so I can reload it if needed.

	int getWidth()        { return width; }
	int getHeight()       { return height; }
	bool getTransparency(){ return hasTransparency; }
	GLuint getTexId()     { return texId; }

	bool error;
};

//*****************************************
// Cubemap Texture
//
//*****************************************

class CubemapGl : public TextureGL
{
protected:
    string m_fileNames[6];

public:
	CubemapGl( const std::string& data_path );
	CubemapGl( const std::string filename[6] );

	void Load();
};
