#include "TextureGL.h"
#include "tga.h"
#include <iostream>
using namespace std;
#include "CommonLibrary.h"
#include "assert.h"
#include <algorithm>
#include "FullScreenEffectShader.h"
#include "Renderer.h"
#include "FramebufferObject.h"
#include "OpenGLModel.h"

using namespace glm;

// Helpers function
GLuint makeTextureRGBA( int sx, int sy,unsigned char* data);
GLuint makeTexture8Bits( int sx, int sy,unsigned char* data);

//*******************************
// Texture Manager
//
//*******************************

TextureManager* TextureManager::thisObject = NULL;

TextureManager::TextureManager()
{
	TEXTURE_PATH = "PATH/NOT/SET/";
}

void TextureManager::setTexturePath(const std::string& path)
{
	TEXTURE_PATH = path;
}

TextureManager* TextureManager::get()
{
	if( thisObject == NULL )
	{
		thisObject = new TextureManager();
	}
	return thisObject;
}

TextureGL* TextureManager::loadTexture(  const std::string&  name,  const std::string&  filename )
{
	TextureGL* newTex = getTexture(name);
	if( newTex != NULL )
	{
		return newTex;
	}
	newTex = new TextureGL();
	newTex->CreateFileTexture( name, TEXTURE_PATH + filename );
	textures.push_back(newTex);

	cout<<textures.size()<<" textures loaded"<<endl;
	return newTex;
}

TextureGL* TextureManager::loadCubemapTexture( const std::string& name, const std::string data_path )
{
	TextureGL* newTex = getTexture(name);
	if( newTex != NULL )
	{
		return newTex;
	}
	newTex = new CubemapGl(data_path);
	textures.push_back(newTex);
	cout<<textures.size()<<" textures loaded"<<endl;
	return newTex;
}

TextureGL* TextureManager::loadCubemapTexture( const std::string& name, const std::string filename[6] )
{
	TextureGL* newTex = getTexture(name);
	if( newTex != NULL )
	{
		return newTex;
	}
	newTex = new CubemapGl(filename);
	textures.push_back(newTex);
	cout<<textures.size()<<" textures loaded"<<endl;
	return newTex;
}

TextureGL* TextureManager::getTexture( const string& name )
{
	for( auto it = textures.begin(); it != textures.end(); it++ )
	{
		if( (*it) == NULL )
		{
			cout<<"Error it"<<endl;
			return NULL;
		}
		if( (*it)->name == name )
		{
			cout<<"found texture "<<name<<endl;
			return (*it);
		}
	}
	return NULL;
}

TextureGL* TextureManager::createRGBATexture( const std::string&  name, const vec4& color, int sizeX, int sizeY )
{
	TextureGL* tex = getTexture(name);
	if( tex != NULL )
	{
		return tex;
	}

	tex = new TextureGL();
	tex->CreateRGBATexture(name, color, sizeX, sizeY );
	textures.push_back(tex);
	return tex;
}

TextureGL* TextureManager::createDepthTexture( const std::string& name, int sizeX, int sizeY )
{
	TextureGL* tex = getTexture(name);
	if( tex != NULL )
	{
		return tex;
	}

	tex = new TextureGL();
	tex->CreateDepthTexture(name,sizeX,sizeY);
	textures.push_back(tex);
	return tex;
}

TextureGL* TextureManager::CombinedTexture( const std::string& name, TEXTURE_COMBINATION combine, TextureGL* tex1, TextureGL* tex2 )
{
	TextureGL* newTex = getTexture(name);
	if( newTex != NULL )
	{
		return newTex; // already generated ? or similar name, too bad.
	}
	newTex = new TextureGL();

	TextureMixShader* shader = (TextureMixShader*)ShaderManager::get()->getShader(TEXTURE_MIX_SHADER);
	assert(shader != NULL );

	newTex->CreateRGBATexture( name, vec4(0,0,0,0), std::min(tex1->width,2048), std::min(tex1->height,2048) );
	TextureGL* depthTex = new TextureGL();
	depthTex->CreateDepthTexture("depthTexTemp", newTex->width, newTex->height );

	FramebufferObject* MixFbo = new FramebufferObject();
	MixFbo->AttachTexture(GL_TEXTURE_2D, newTex->getTexId(), GL_COLOR_ATTACHMENT0);
	MixFbo->AttachTexture(GL_TEXTURE_2D, depthTex->getTexId(), GL_DEPTH_ATTACHMENT );

	DynamicMesh* mesh = new DynamicMesh(4,2);
	vec3 vert[4] = { vec3(0,0,-10),vec3(1,0,-10), vec3(1,1,-10), vec3(0,1,-10) };
	vec2 uv[4]   = { vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,1) };
	unsigned short tri[2*3] = { 0,1,2, 0,2,3 };
	mesh->updateGeometry( vert, uv, 4, tri, 2  );

	MixFbo->Bind();
	glViewport(0,0,newTex->width, newTex->height);
	glClearColor(0.0,0.0, 0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	ShaderParams::get()->objectMatrix     = mat4(1.0);
	ShaderParams::get()->viewMatrix       = mat4(1.0);
	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.001f,100.0f);

	shader->enable( *ShaderParams::get() );
	shader->uTextureSampler1->setValue( tex1->getTexId() );
	shader->uTextureSampler2->setValue( tex2->getTexId() );
	shader->uTextureMix->setValue( combine == TEXTURE_COMBINATION::MIX );
	shader->uTextureMul->setValue( combine == TEXTURE_COMBINATION::MULTIPLY );
	shader->uTextureDiff->setValue( false );//combine == TEXTURE_COMBINATION::DIFFERENCE );

	shader->Draw( &(mesh->mesh) );
	shader->disable();

	MixFbo->Disable();
	glViewport(0,0,ShaderParams::get()->win_x, ShaderParams::get()->win_y);

	delete MixFbo;
	delete mesh;
	return newTex;
}

void TextureManager::deleteTexture( TextureGL* tex )
{
	if( tex == NULL ) return;

	auto it = find( textures.begin(), textures.end(), tex );
	if( it != textures.end() )
	{
		delete (*it);
		textures.erase( it );
	}
}

void TextureManager::reloadTextures()
{
	int nbTextures = 0;
	for( auto it = textures.begin(); it != textures.end(); it++ )
	{
		(*it)->Load();
		nbTextures++;
	}
	LOGT("Texture", "reloaded %d textures", nbTextures);
}

//*******************************
// TextureGL
//
//*******************************

TextureGL::TextureGL()
{
	textureType = TEXTURE_TYPE::UNDEFINED;
}

void TextureGL::CreateFileTexture( const std::string& n, const std::string& file )
{
	textureType = TEXTURE_TYPE::TEXTURE_FILE;
	name = n;
	filename = file;
	Load();
}

void TextureGL::CreateRGBATexture( const std::string&  n, const vec4& c, int w, int h )
{
	// create a new texture of the given color
	textureType = TEXTURE_TYPE::TEXTURE_COLOR;
	name = n;
	hasTransparency = color.w < 1.0f;
	width           = w;
	height          = h;
	bpp             = 32;
	color           = c;
	Load();
}

void TextureGL::CreateDepthTexture( const std::string& n, int sizeX, int sizeY )
{
	textureType = TEXTURE_TYPE::DEPTH;
	name = n;
	width         = sizeX;
	height        = sizeY;
	bpp           = 32;
	Load();
}

TextureGL::~TextureGL()
{
	textureType = TEXTURE_TYPE::UNDEFINED;
	texId = -1;
	if( glIsTexture(texId) )
	{
		glDeleteTextures(1,&texId);
	}
}

void TextureGL::Load()
{
	switch( textureType )
	{
		case TEXTURE_TYPE::UNDEFINED:
		{
			assert(false);
		}
		break;

		case TEXTURE_TYPE::TEXTURE_FILE:
		{
			string format = filename.substr( filename.size()-3 , string::npos );
			error         = true;
			width         = 0;
			height        = 0;
			bpp           = 0;
			unsigned char* pixels = 0;

			if( true || format == "tga" )
			{
				bool hasAlpha = false; // return true if the alpha is not 1.0 somewhere in the texture.


				pixels = readTGA(filename.c_str(), width, height, bpp, hasAlpha);
				hasTransparency = hasAlpha;
				if( hasTransparency )
				{
					cout<<filename<<" texture has transparency"<<endl;
				}
			}
			else
			{
				cout<<"format non supporte pour "<<filename<<endl;
				return;
			}

			if( bpp == 16 ||  bpp == 24 || bpp == 32 )
			{
				texId = makeTextureRGBA(width,height,pixels);
			}
			else if( bpp == 8 )
			{
				texId = makeTexture8Bits(width, height, pixels);
			}
			else
			{
				LOGE("probleme de texture");
				cout<<"format non supporte pour "<<filename<<" (transparence alpha) "<<endl;
				return;
			}

			error = false;
			delete[] pixels;
		}
		break;

		case TEXTURE_TYPE::TEXTURE_COLOR:
		{
			unsigned char* pixels = new unsigned char[4*width*height];
			for( int i=0; i< width*height; i++ )
			{
				pixels[i*4]   = color.x * 255.0f;
				pixels[i*4+1] = color.y * 255.0f;
				pixels[i*4+2] = color.z * 255.0f;
				pixels[i*4+3] = color.w * 255.0f;
			}
			texId = makeTextureRGBA(width,height,pixels);
			error = false;
			delete pixels;
		}
		break;

		case TEXTURE_TYPE::DEPTH:
		{
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		break;
	};
}

//*****************************************
// Helper functions
//
//*****************************************

GLuint makeTextureRGBA( int width, int height,unsigned char* data)
{
	//http://www.opengl.org/wiki/Common_Mistakes
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGE("ERROR 1 %d",errCode);
	}

	int num_mipmaps = 4;
	GLuint textureID = 0;
/*
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexStorage2D(GL_TEXTURE_2D, num_mipmaps, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0​, 0, 0, width​, height​, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate num_mipmaps number of mipmaps here.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
*/


	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#ifdef __ANDROID__
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#endif

	if ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGE("ERROR 2 %d",errCode);
	}

	/*

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	*/
/*
	// I HAVE TO COMMENT THAT glTexStorage2D(GL_TEXTURE_2D, num_mipmaps, GL_RGBA8, sx, sy);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sx, sy, GL_RGBA, GL_UNSIGNED_BYTE, data );

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,sx,sy,0,GL_RGBA8,GL_UNSIGNED_BYTE, data );

	glGenerateMipmap(GL_TEXTURE_2D);  //Generate num_mipmaps number of mipmaps here.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
*/
	/*
	glTexImage2D (
		GL_TEXTURE_2D, 	//Type : texture 2D
		0, 	//Mipmaps
		GL_RGBA, 	//Couleurs : 4
		sx, 	//Largeur
		sy, 	//Hauteur
		0, 	//Largeur du bord : 0
		GL_RGBA	, 	//Format : RGB
		GL_UNSIGNED_BYTE, 	//Type des couleurs
		data 	//Addresse de l'image
	);
	//glGenerateMipmap(GL_TEXTURE_2D);  //Generate num_mipmaps number of mipmaps here.

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
*/
	return textureID;

}

GLuint makeTexture8Bits( int sx, int sy,unsigned char* data)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D (
		GL_TEXTURE_2D, 	//Type : texture 2D
		0, 	//Mipmap : aucun
		GL_RGB, 	//Couleurs : 4
		sx, 	//Largeur
		sy, 	//Hauteur
		0, 	//Largeur du bord : 0
		GL_LUMINANCE	, 	//Format : GL_LUMINANCE (niveau de gris)
		GL_UNSIGNED_BYTE, 	//Type des couleurs
		data 	//Addresse de l'image
	);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return textureID;
}


//*****************************************
// Cubemap Texture
//
//*****************************************

CubemapGl::CubemapGl(const std::string& data_path) : TextureGL()
{
	textureType = TEXTURE_TYPE::CUBEMAP;

	const char* DATA_PATH = "";
	m_fileNames[0] = data_path + string("spacebox_right.png");
	m_fileNames[1] = data_path + string("spacebox_left.png");
	m_fileNames[2] = data_path + string("spacebox_top.png");
	m_fileNames[3] = data_path + string("spacebox_bottom.png");
	m_fileNames[4] = data_path + string("spacebox_front.png");
	m_fileNames[5] = data_path + string("spacebox_back.png");

	Load();
}

CubemapGl::CubemapGl( const std::string filename[6] ) : TextureGL()
{
	textureType = TEXTURE_TYPE::CUBEMAP;

	for( int i=0; i<6; i++ )
	{
		m_fileNames[i] = filename[i];
	}

	Load();
}

void CubemapGl::Load()
{
	static const GLenum types[6] =
	{
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	 	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	 	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	 	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	  	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
/*
    m_fileNames[0] = string(DATA_PATH) + string("sp3right.tga");
    m_fileNames[1] = string(DATA_PATH) + string("sp3left.tga");
    m_fileNames[2] = string(DATA_PATH) + string("sp3top.tga");
    m_fileNames[3] = string(DATA_PATH) + string("sp3bot.tga");
    m_fileNames[4] = string(DATA_PATH) + string("sp3front.tga");
    m_fileNames[5] = string(DATA_PATH) + string("spaceback.tga");
*/

 
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
#ifndef __ANDROID__
	glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
#endif

    for (unsigned int level = 0 ; level < 6 ; level++) {

		int width, height, bpp;
		bool hasAlpha = false;
		unsigned char* data = readTGA(m_fileNames[level].c_str(), width, height, bpp, hasAlpha);
		assert(data != 0 );

    	// unflip the image vertically
    	for( int i = 0; i< width; i++ )
    	{
    		for( int j=0; j< height/2; j++ )
    		{
    			for( int k=0; k<4; k++ )
    			{
    				char temp = data[4 * (j*width + i) +k];
    				data[4 * (j*width + i) +k] = data[4* ((height-1-j)*width + i) +k];
    				data[4* ((height-1-j)*width + i) +k] = temp;
    			}
    		}
    	}

 /*   	// unflip the image horizontally
    	for( int j=0; j< height; j++ )
    	{
    		for( int i = 0; i< width/2; i++ )
    		{
    			for( int k=0; k<4; k++ )
    			{
    				char temp = data[4* (j*width + i) +k];
    				data[4* (j*width + i) +k] = data[4* ((j*width) + (width-i)) +k];
    				data[4* ((j*width) + (width-i)) +k] = temp;
    			}
    		}
    	}
*/
        glTexImage2D(types[level], 0, GL_RGBA, width, height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, data);



        GLenum errCode;
    	while (( errCode = glGetError()) != GL_NO_ERROR) {
    		LOGT("START","Error Skybox::Skybox glTexImage2D %d",errCode);
    		//assert(false);
    	}

        delete data;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R_OES, GL_CLAMP_TO_EDGE);

}
