#include "GLText.h"
#include "BitmapFont.h"

#include "TextureGL.h"
#include <iostream>
#include "Material.h"
#include "ShaderParams.h"
#include "OpenGLModel.h"
#include "tga.h"
#include "Tweaker.h"
#include "TTFont.h"

using namespace glm;

GLFont::GLFont( const std::string& name, const char* directory, const char* filename ) : fontName(name)
{
	font = new BitmapFont( directory, filename );

	// load the character pages textures
	for( unsigned int i=0; i< font->texName.size() ; i++ )
	{
		string texPath = string(directory) + string(font->texName[i]);
		//texPath = string(DATA_PATH + string("cube.tga") );

		TextureGL* newPage = TextureManager::get()->loadTexture(  filename, texPath.c_str() );
		if( newPage->error )
		{
			cout<<"error couldn't load texture "<<texPath<<endl;
			return;
		}
		pages.push_back( newPage );
	}
	assert(pages.size()==1);

	textMat = new MaterialColoredText();
	((MaterialColoredText*)textMat)->setTextureID( pages[0]->getTexId() );

	size = 32.0f / 256.0f ; // see the .fnt file
	interline = 0.0f / 256.0f;

	// the total height of this text
	sizeY = 0;

	defaultColor = vec4(1,1,1,1);
	modifierStack.push(defaultColor);

/*
 *  SAMPLE CODE FOR Sampler2DArray (not available on ES 2.0)
 *
  	textMat = new MaterialTextureArray();

	GLsizei width = 256;
	GLsizei height = 256;
	int bpp = 0;
	bool hasAlpha;
	//unsigned char* data =
	unsigned char* data = readTGA("/media/E/projets/Antigravity/Data/.tga", width, height, bpp, hasAlpha);
	unsigned char* data2 = readTGA("/media/E/projets/Antigravity/Data/bitmap_font/Arial/arial_1.tga", width, height, bpp, hasAlpha);

	int nbTextures = 2;

#ifdef __ANDROID__
#else

	glGenTextures(1,&tex2D);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex2D);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, width, height, nbTextures, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL );
	glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE, data );
	glTexSubImage3D (GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, width, height, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE, data2 );

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_ARRAY,0);

	delete[] data;
	delete[] data2;

	((MaterialTextureArray*)textMat)->setTextureID( tex2D);
#endif
*/
}

DynamicMesh* GLFont::createGLString( const wstring& sentence )
{
	// dessine une texture :
	//textureShader->enable(mat4(1.0), viewMatrix, projectionMatrix );
	vec3*  vertices    = new vec3[ sentence.size() * 4 ];
	vec2*  texCoord    = new vec2[ sentence.size() * 4 ];
	vec4*  colorBuffer = new vec4[ sentence.size() * 4 ]; // 1 par sommet... relou mais voir avec glVertexBindingDivisor(); (opengl 3 ou 4...)
	unsigned short* triangles = new unsigned short[ sentence.size() * 2 * 3];

	float Xoffset = 0.0f;
	float Yoffset = 0.0f;

	int nbCharacters = 0; // not necessarily equal to i because we skip some characters (like \n)
	for( unsigned int i=0; i< sentence.length(); i++ )
	{
		vec4 color = modifierStack.top();

		// check for special characters :
		// eventually I will have to do a basic parser for colors and stuff !
		if( sentence[i] == '\n' )
		{
			Xoffset = 0.0f;
			Yoffset += size + interline;

			continue;
		}
		else if( sentence[i] == '|' )
		{
			// look for the modifier (color, etc)
			if( sentence.substr(i+1,sizeof(L"BLACK")).find( L"BLACK" ) == 0 )
			{
				modifierStack.push( vec4(0,0,0,1));
			}
			else if( sentence.substr(i+1,sizeof(L"BLUE")).find( L"BLUE" ) == 0 )
			{
				modifierStack.push( vec4(0.3,0.3,1,1));
			}
			else if( sentence.substr(i+1,sizeof(L"GREY")).find( L"GREY" ) == 0 )
			{
				modifierStack.push( vec4(0.7,0.7,0.7,1) );
			}
			else if( sentence.substr(i+1,sizeof(L"END")).find( L"END" ) == 0 )
			{
				if( modifierStack.size() > 0)
					modifierStack.pop();
			}

			size_t nextLimit = sentence.find( L"|", i+1 );
			if( nextLimit == sentence.npos )
			{
				assert( false ); // texte mal forme
				nextLimit = i;
			}
			i += nextLimit-i; // skip the marker
			continue;
		}

		int c = (int)sentence[i];
		int CharIndex = font->utfKey[c];
		if( CharIndex < 0 ) // char non present dans notre police, que faire ?
		{
			Xoffset += size;
			std::cerr<<" character "<<sentence[i]<<" (utf "<<c<<", char "<<(char)c<<" ) not present in font"<<endl;
			continue;
		}
		CharacterFont& cF = font->characterMap[CharIndex];

		float yoff = - cF.h - cF.yoffset/256.0f;
		float xoff = cF.xoffset/256.0f;
		Xoffset += xoff;

		vertices[nbCharacters*4]   = vec3(  Xoffset,      yoff- Yoffset ,         0.0f   );
		vertices[nbCharacters*4+1] = vec3(  Xoffset+cF.w, yoff- Yoffset  ,         0.0f  );
		vertices[nbCharacters*4+2] = vec3(  Xoffset+cF.w, yoff- Yoffset  + cF.h  , 0.0f  );
		vertices[nbCharacters*4+3] = vec3(  Xoffset,      yoff- Yoffset  + cF.h  , 0.0f  );

		texCoord[nbCharacters*4]   = vec2( cF.x,        cF.y - cF.h  );
		texCoord[nbCharacters*4+1] = vec2( cF.x + cF.w, cF.y - cF.h  );
		texCoord[nbCharacters*4+2] = vec2( cF.x + cF.w, cF.y   );
		texCoord[nbCharacters*4+3] = vec2( cF.x,        cF.y   );

		colorBuffer[nbCharacters*4] = color ;
		colorBuffer[nbCharacters*4+1] = color ;
		colorBuffer[nbCharacters*4+2] = color ;
		colorBuffer[nbCharacters*4+3] = color ;

		triangles[(nbCharacters*2)*3]     = nbCharacters*4;
		triangles[(nbCharacters*2)*3+1]   = nbCharacters*4+1;
		triangles[(nbCharacters*2)*3+2]   = nbCharacters*4+2;

		triangles[(nbCharacters*2+1)*3]   = nbCharacters*4;
		triangles[(nbCharacters*2+1)*3+1] = nbCharacters*4+2;
		triangles[(nbCharacters*2+1)*3+2] = nbCharacters*4+3;

		Xoffset += cF.w;
		if( cF.w + Xoffset >= 1.0f )
		{
			Yoffset += size + interline;
			Xoffset = 0.0f;
		}

		nbCharacters++;
	}

	sizeY = Yoffset + 0.025;
	sizeX = Xoffset;

	// create the letters:
	//cout<<vertices.size()<<" letters in the string "<<endl;//\""<<sentence<<"\""<<endl;
	DynamicMesh* lettersMesh = NULL;
	lettersMesh = new DynamicMesh(nbCharacters*4,nbCharacters*2);
	lettersMesh->updateGeometry( vertices, texCoord, nbCharacters*4, triangles, nbCharacters*2 );
	lettersMesh->updateBuffer( colorBuffer, nbCharacters*4, lettersMesh->mesh.colorBuffer, true );
	return lettersMesh;
}


//********************************************************
// A mesh holding a text string
//
//********************************************************

DynamicText::DynamicText()
{
	glFont = FontManager::get()->getCurrentFont();

	rotation = vec3(0,0.0f,0);
	position = vec3(0.00,0.5,-10);
	scale = vec3(0.23,0.23,0.23);


#if defined(USE_TWEAKER)
	Tweaker::get()->addTweakable( new FloatTweakable( "text scale",
			[this](float f){ scale.x = scale.y = scale.z = f; }, 1.0f, 0.0f,5.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "text posX",
			[this](float f){ position.x = f; }, position.x, -1.0f,1.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "text posY",
			[this](float f){ position.y = f; }, position.y, -1.0f,1.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "text rotation y",
			[this](float f){ rotation.y = f; }, 0.0f, -180.0f,180.0f ));

#endif

	sizeY = 0.0f;
	sizeX = 0.0f;
}

DynamicText::DynamicText(MyNamespace::Font* font ) :  glFont(font)
{
	rotation = vec3(0,0.0f,0);
	position = vec3(0.05,0.20,-10);
	scale = vec3(0.23,0.23,0.23);
	sentence = L"";

	lettersMesh = NULL;
	sizeY = 0.0f;
	sizeX = 0.0f;

#if defined(USE_TWEAKER)
	Tweaker::get()->addTweakable( new FloatTweakable( "text scale",
			[this](float f){ scale.x = scale.y = scale.z = f; }, 1.0f, 0.0f,5.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "text posX",
			[this](float f){ position.x = f; }, position.x, -1.0f,1.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "text posY",
			[this](float f){ position.y = f; }, position.y, -1.0f,1.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "text rotation y",
			[this](float f){ rotation.y = f; }, 0.0f, -180.0f,180.0f ));

#endif
}

DynamicText::DynamicText( const wstring& s ) : sentence(s)
{
	glFont = FontManager::get()->getCurrentFont();

	rotation = vec3(0,0.0f,0);
	position = vec3(0.00,0.5,-10);
	scale = vec3(0.23,0.23,0.23);


#if defined(USE_TWEAKER)
	Tweaker::get()->addTweakable( new FloatTweakable( "text scale",
			[this](float f){ scale.x = scale.y = scale.z = f; }, 1.0f, 0.0f,5.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "text posX",
			[this](float f){ position.x = f; }, position.x, -1.0f,1.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "text posY",
			[this](float f){ position.y = f; }, position.y, -1.0f,1.0f ));

	Tweaker::get()->addTweakable( new FloatTweakable( "text rotation y",
			[this](float f){ rotation.y = f; }, 0.0f, -180.0f,180.0f ));

#endif

	sizeX = 0.0f;
	sizeY = 0.0f;

	createGLString( sentence );
}

DynamicText::~DynamicText()
{
	if( lettersMesh != NULL )
	{
		delete lettersMesh;
		lettersMesh = NULL;
	}
}

void DynamicText::reloadGLData()
{
	if( lettersMesh != NULL )
	{
		delete lettersMesh;
		lettersMesh = NULL;
	}

	if( sentence.size() > 0 )
	{
		createGLString(sentence);
	}
}

void DynamicText::createGLString( const wstring& s )
{
	sentence = s;
	lettersMesh = glFont->createGLString( sentence );

	sizeY = glFont->sizeY;
	sizeX = glFont->sizeX;
}


void DynamicText::Draw(MATERIAL_DRAW_PASS Pass)
{
	if( lettersMesh == NULL )
		return;

	Shader* shader = glFont->textMat->getShader(Pass);
	if( shader != NULL )
	{
		DrawCall drawcall;
		drawcall.Pass     = Pass;

		//position.z = -1;
		//mat4 tileScaleRot = glm::rotate( mat4(1.0), 90.0f, vec3(1,0,0)) * glm::rotate( mat4(1.0), rotation.y, vec3(0,1,0)) *  glm::scale( mat4(1.0), scale );
		//mat4 objectmat =   glm::translate( mat4(1.0), position ) * tileScaleRot;
		mat4 objectmat =  glm::translate( mat4(1.0), position ) * glm::scale( mat4(1.0), scale );

	//	mat4 tileScaleRot = glm::scale( mat4(1.0), scale ) * glm::rotate( mat4(1.0), rotation.x, vec3(1,0,0) );// * glm::rotate( mat4(1.0), rotation.x, vec3(1,0,0) ) ;
	//	mat4 objectmat =  glm::translate( mat4(1.0), position )  * tileScaleRot;

		drawcall.modelMat = objectmat;
		drawcall.viewMat  = ShaderParams::get()->viewMatrix;
		drawcall.projMat  = ShaderParams::get()->projectionMatrix;

		if( shader != NULL )
		{
			drawcall.material = glFont->textMat;
			drawcall.mesh     = &(lettersMesh->mesh);
			drawcall.hasTransparency = true;
			drawcall.transparencyMode = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
			Renderer::get()->draw( drawcall );
		}
	}
}



