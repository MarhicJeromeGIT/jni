#include "TTFont.h"

#include "fichiers.h"
#include <iostream>
using namespace std;
#include "CommonLibrary.h"
#include "assert.h"

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include "OpenGLModel.h"

using namespace glm;

#define BOLD_OFFSET   050000
#define ITALIC_OFFSET 100000

#define FRANKY_OFFSET 1000000

//*******************************************//
// Font Manager
//
//*******************************************//

FontManager* FontManager::thisObject = NULL;

FontManager::FontManager()
{
	ttfont = new TTFont();
}
FontManager* FontManager::get()
{
	if( thisObject == NULL )
	{
		thisObject = new FontManager();
	}
	return thisObject;
}

MyNamespace::Font* FontManager::getCurrentFont()
{
	return ttfont;
}






struct FontFamily // always the order (update : Ca veut dire quoi ce commentaire ? -_- )
{
	FT_Library library;

	FontFamily(){};
	FontFamily(FT_Library lib, const string& fontpath) : library(lib)
	{
		face_path[FONTFACES::NORMAL] = fontpath;

		long size = 0;
	 	unsigned char* buffer = readBuffer(fontpath.c_str(), size);
	 	FT_Error error = FT_New_Memory_Face( library, buffer, size, 0, &faces[FONTFACES::NORMAL] );
		//FT_Error error = FT_New_Face( library, face_path[NORMAL].c_str() , 0, &faces[FONTFACES::NORMAL] );

		if ( error  )
		{
			cout<<"Error loading font, abort."<<fontpath<<endl;
			assert(false);
			return;
		}


		for( int i=FONTFACES::NORMAL+1; i<FONTFACES::MAX_FACES; i++ )
		{
			face_path[i] = face_path[FONTFACES::NORMAL];
			faces[i]     = faces[FONTFACES::NORMAL];
		}

	}
	void setFont( FONTFACES face, const string& fontpath  )
	{
		/*
		face_path[face] = fontpath;
		FT_Error error = FT_New_Face( library, face_path[face].c_str() , 0, &faces[face] ); // FT_New_Memory_Face for android
		if ( error  )
		{
			cout<<"Error loading font, abort."<<fontpath<<endl;
			assert(false);
			return;
		}*/
	}

	string face_path[FONTFACES::MAX_FACES];
	FT_Face faces[FONTFACES::MAX_FACES];
};


FontFamily fonts[FONTFAMILY::MAX_FONTS];

void TTFont::setFontColor( const vec4& col )
{
	FontModifier font = modifierStack.top();
	modifierStack.pop();
	font.color = col;
	modifierStack.push(font);
}

void TTFont::setDefaultFontFamily( FONTFAMILY family )
{
	FontModifier font = modifierStack.top();
	modifierStack.pop();
	font.family = family;
	modifierStack.push(font);
}

void TTFont::createFontFaces()
{
	FontFamily droidface(library, FONT_PATH + string("DroidSans.ttf") );
	droidface.setFont( FONTFACES::BOLD, FONT_PATH + string("DroidSans-Bold.ttf") );
	droidface.setFont( FONTFACES::ITALIC, FONT_PATH + string("DroidSerif-BoldItalic.ttf") );
	fonts[FONTFAMILY::DROID] = droidface;

	FontFamily cinnamon(library, FONT_PATH + string("cinnamon cake.ttf") );
	fonts[FONTFAMILY::CINNAMON] = cinnamon;

	FontFamily nanum( library, FONT_PATH + string("NanumMyeongjo.ttf") );
	fonts[FONTFAMILY::NANUM] = nanum;

	fonts[FONTFAMILY::FRANKY] = cinnamon;
}

void TTFont::setFontPath(const std::string& path)
{
	FONT_PATH = path;
}

std::string TTFont::FONT_PATH = "FONT/PATH/NOT/SET";

TTFont::TTFont()
{
	LOGE("Loading fonts");

	 // http://www.freetype.org/freetype2/docs/tutorial/step1.html
	 FT_Error error = FT_Init_FreeType( &library );
	 if ( error )
	 {
		 cout<<"error init freetype"<<endl;
	 }
	 memset(array,255, texSize*texSize*sizeof(unsigned char));

	 createFontFaces();

	 // error = FT_Attach_File( face,  DATA_PATH "letterng-medium.afm" );
	 //if( error )
	// {
	//	 cout<<"couldn't apply kerning infos"<<endl;
	 //}

	 error = FT_Set_Pixel_Sizes( fonts[NANUM].faces[NORMAL],  450,  450 );
	 if ( error == FT_Err_Unknown_File_Format )
     {
		 cout<<"Error with FT_Set_Pixel_Sizes"<<endl;
	 }

	 FontModifier defaultFont;
	 modifierStack.push(defaultFont); // default font modifier

	interline = 0.02f;

	textureId = -1;
	textMat = new MaterialColoredText();
	reloadGLData();
}

void TTFont::reloadGLData()
{
	memset(array,0, 4*texSize*texSize*sizeof(unsigned char));
	currentX = 4;
	currentY = 0;
	maxRows = 0;

	// just delete everything
	//	if( glIsTexture(textureId) )
	//		glDeleteTextures(1,&textureId);

	glGenTextures(1, &textureId); // the texture holding our glyphs
	glBindTexture( GL_TEXTURE_2D, textureId );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	((MaterialColoredText*)textMat)->setTextureID( textureId );

	utfKey.clear();
	characterMap.clear();
}

DynamicMesh* TTFont::createGLString( const wstring& sentence )
{

	// First generate a texture with all the glyphs I need, and record their position and size and all,
	// then create a VBO with the appropriate tex coords.
	 for( int i=0; i< sentence.size(); i++ )
	 {
		 FontModifier currentModifier = modifierStack.top();
		 FT_Face face = fonts[currentModifier.family].faces[currentModifier.face];

		 FT_Error error;
		 // taille en 1/64eme de 'points' ( 1 point = 1/72 pouce...super)
		 // DPI : Nexus = 323
		 error =  FT_Set_Char_Size(face, 48*64, 0, 96, 96 );
		 assert( error == 0 );

		 if( sentence[i] == '|' )
		 {
			// look for the modifier (color, etc)
			if( sentence.substr(i+1,sizeof(L"BLACK")).find( L"BLACK" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(0,0,0,1);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"BLUE")).find( L"BLUE" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(0.3,0.3,1,1);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"GREEN")).find( L"GREEN" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(0.1,1.0,0.1,1);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"RED")).find( L"RED" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(1.0,0.0,0.1,1);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"YELLOW")).find( L"YELLOW" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(1.0,1.0,0,1);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"GREY")).find( L"GREY" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(0.7,0.7,0.7,1);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"LINK")).find( L"LINK" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(0.3,0.3,0.1,1);
				modifier.underlined = true;
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"BOLD")).find( L"BOLD" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.face = FONTFACES::BOLD;
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"ITALIC")).find( L"ITALIC" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.face = FONTFACES::ITALIC;
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"FRANKY")).find( L"FRANKY" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.family = FONTFAMILY::FRANKY;
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"END")).find( L"END" ) == 0 )
			{
				if( modifierStack.size() > 0)
					modifierStack.pop();
				cout<<"STACKSIZE = "<<modifierStack.size()<<endl;
			}
			else
			{
				cout<<"modifier non reconnu "<<sentence.substr(i,10).c_str()<<endl;
				FontModifier topStack = modifierStack.top();
				modifierStack.push(topStack);
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

		 // check if we've already registered that letter, or registers it :
		 unsigned int charcode = (unsigned int) sentence[i];
		 int offset = 0;
		 if( currentModifier.family == FONTFAMILY::FRANKY )
		 {
			 offset += FRANKY_OFFSET;
		 }

		 if( currentModifier.face == FONTFACES::BOLD )
		 {
			 offset += BOLD_OFFSET;
		 }
		 else if( currentModifier.face == FONTFACES::ITALIC )
		 {
			 offset += ITALIC_OFFSET;
		 }

		 auto it = utfKey.find( charcode + offset );
		 if( it != utfKey.end() ) // we already registered that
			 continue;

		 //cout<<"registering char "<<(char)sentence[i]<<"( unicode "<<charcode<<" )"<<endl;

		 utfKey.insert( pair<int,int>( charcode + offset, characterMap.size() ) );



		 error = FT_Load_Char( face, charcode, FT_LOAD_NO_BITMAP );
		 if( error != 0)
		 {
			 cout<<"error with char "<<charcode<<": "<<error<<endl;
			 continue;
		 }

		 error = FT_Load_Char( face, charcode, FT_LOAD_DEFAULT );
		 if( error != 0)
		 {
			 cout<<"error with char "<<charcode<<": "<<error<<endl;
			 continue;
		 }

		 // The glyph outline :
		FT_Stroker stroker;
		FT_Stroker_New(library, &stroker);
		FT_Stroker_Set(stroker,
					   (int)(4.0f * 64),
					   FT_STROKER_LINECAP_ROUND,
					   FT_STROKER_LINEJOIN_ROUND,
					   0);

		FT_Glyph glyph_stroked;
		if (FT_Get_Glyph(face->glyph, &glyph_stroked) == 0)
		{
			FT_Glyph_StrokeBorder(&glyph_stroked, stroker, false, 1);
			FT_Glyph_To_Bitmap( &glyph_stroked,  FT_RENDER_MODE_NORMAL, 0, false );
		}

		// The basic glyph :
		 bool isBitmap = fonts[currentModifier.family].faces[currentModifier.face]->glyph->format == FT_GLYPH_FORMAT_BITMAP;

		// assert( !isBitmap );
		 //cout<<"bitmap format ? : "<< isBitmap <<endl;
		 if( !isBitmap )
		 {
			 error = FT_Render_Glyph( face->glyph,  FT_RENDER_MODE_NORMAL );
			 if(error!=0 )
			 {
				 cout<<"error FT_Render_Glyph "<<charcode<<": "<<error<<endl;
				 continue;
			 }
		 }

		FT_Bitmap bmp_stroke = ((FT_BitmapGlyph)glyph_stroked)->bitmap;
		FT_Bitmap bmp = face->glyph->bitmap;

//		 cout<<"bitmap info"<<endl;
//		 cout<<"w,h = "<<bmp.width<<","<<bmp.rows<<endl;
//		 cout<<"pitch = "<<bmp.pitch<<endl; // number of bytes taken by one bitmap row, including padding
//		 cout<<"gray "<<bmp.num_grays<<endl;
//		 cout<<"pixel_mode "<<(int)bmp.pixel_mode<<endl;

		 maxRows = std::max(maxRows, bmp_stroke.rows);
		 if( currentX + bmp.width > texSize )
		 {
			 currentX = 2;
			 currentY += maxRows + 4;

			 assert( currentY < texSize-bmp_stroke.rows );
		 }

		 for( int i=0; i < bmp_stroke.rows; i++ )
		 {
			 for( int j=0; j< bmp_stroke.width; j++ )
			 {
				 //cout<<(int)bmp.buffer[i*bmp.width+j]<<" ";
				 //array[i+currentY][currentX+j][0] = 0;//bmp_stroke.buffer[i*bmp.width+j];
				 array[i+currentY][currentX+j][1] = bmp_stroke.buffer[i*bmp_stroke.width+j];
			 }
		 }

		 int bmp_offX = (bmp_stroke.width - bmp.width)/2;
		 int bmp_offY = (bmp_stroke.rows - bmp.rows)/2;

		 for( int i=0; i< bmp.rows; i++ )
		 {
			 for( int j=0 ; j< bmp.width; j++ )
			 {
				 //cout<<(int)bmp.buffer[i*bmp.width+j]<<" ";
				 array[bmp_offY+i+currentY][bmp_offX+currentX+j][0] = bmp.buffer[i*bmp.width+j];
				 //array[bmp_offY+i+currentY][bmp_offX+currentX+j][1] += bmp.buffer[i*bmp.width+j] > 0 ? 255 : 0;
			 }
			 //cout<<endl;
		 }

		 // register our new character :
		 CharacterFont character;

		 float units = (float) face->units_per_EM * 3.0; // pk * 3.0 ?
		 character.charId = charcode;

		 character.bitmap_x = currentX / (float)texSize;
		 character.bitmap_y = currentY / (float)texSize;
		 character.bitmap_w = bmp_stroke.width / (float)texSize;
		 character.bitmap_h = bmp_stroke.rows / (float)texSize;

		 character.w = (18+face->glyph->metrics.width) / units ;
	     character.h = (face->glyph->metrics.height) / units ;

		 character.xoffset = (  face->glyph->metrics.horiBearingX )/ units  ;
		 character.yoffset = (- face->glyph->metrics.height + face->glyph->metrics.horiBearingY )/ units ; //(float)texSize;
		 character.xadvance = ( face->glyph->metrics.horiAdvance  )   / units ;// / (float)texSize;

		 characterMap.push_back(character);

		 currentX += bmp_stroke.width + 4;
	 }

	 // update the texture with our new glyphs :
	glBindTexture(GL_TEXTURE_2D, textureId);
#ifdef __ANDROID__
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, texSize, texSize, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, array);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, array);
#endif

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	assert ( glGetError() == GL_NO_ERROR);

	//****************************************************************//
	//*** SECOND PART : generate the mesh
	//****************************************************************//

	// dessine une texture :
	//textureShader->enable(mat4(1.0), viewMatrix, projectionMatrix );
	vec3*  vertices    = new vec3[ sentence.size() * 4 ];
	vec2*  texCoord    = new vec2[ sentence.size() * 4 ];
	vec4*  colorBuffer = new vec4[ sentence.size() * 4 ]; // 1 par sommet... relou mais voir avec glVertexBindingDivisor(); (opengl 3 ou 4...)
	unsigned short* triangles = new unsigned short[ sentence.size() * 2 * 3];

	float Xoffset = 0.0f;
	float Yoffset = 0.0f;

	int nbCharacters = 0; // not necessarily equal to i because we skip some characters (like \n)

	cout<<modifierStack.size()<<endl;
	assert( modifierStack.size() == 1 );
	for( int i=0; i< sentence.length(); i++ )
	{
		FontModifier currentModifier = modifierStack.top();
		FT_Face face = fonts[currentModifier.family].faces[currentModifier.face];

		vec4 color = modifierStack.top().color;

		// check for special characters :
		// eventually I will have to do a basic parser for colors and stuff !
		if( sentence[i] == '\n' )
		{
			Xoffset = 0.0f;
			Yoffset += currentModifier.size + interline;

			continue;
		}
		else if( sentence[i] == '\t' )
		{
			Xoffset += currentModifier.size * 4.0f;
			continue;
		}
		else if( sentence[i] == '|' )
		{
			// look for the modifier (color, etc)
			if( sentence.substr(i+1,sizeof(L"BLACK")).find( L"BLACK" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(0,0,0,1);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"BLUE")).find( L"BLUE" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(0.4f,0.3f, 0.8f, 1.0f);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"GREEN")).find( L"GREEN" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(0.1f,1.0f, 0.1f, 1.0f);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"RED")).find( L"RED" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(1.0,0.0,0.1,1);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"YELLOW")).find( L"YELLOW" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(1.0,1.0,0,1);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"GREY")).find( L"GREY" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(0.7,0.7,0.7,1);
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"LINK")).find( L"LINK" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.color = vec4(0.3,0.3,1,1);
				modifier.underlined = true;
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"BOLD")).find( L"BOLD" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.face = FONTFACES::BOLD;
				modifier.size = 1.2f;
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"ITALIC")).find( L"ITALIC" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.face = FONTFACES::ITALIC;
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"FRANKY")).find( L"FRANKY" ) == 0 )
			{
				FontModifier modifier = modifierStack.top();
				modifier.family = FONTFAMILY::FRANKY;
				modifierStack.push(modifier);
			}
			else if( sentence.substr(i+1,sizeof(L"END")).find( L"END" ) == 0 )
			{
				if( modifierStack.size() > 0)
					modifierStack.pop();
			}
			else
			{
				cout<<"modifier non reconnu "<<sentence.substr(i,10).c_str()<<endl;
				FontModifier topStack = modifierStack.top();
				modifierStack.push(topStack);
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

		int offset = 0;
		if( currentModifier.family == FONTFAMILY::FRANKY )
		{
			offset += FRANKY_OFFSET;
		}

		if( currentModifier.face == FONTFACES::BOLD )
		{
			offset += BOLD_OFFSET;
		}
		else if( currentModifier.face == FONTFACES::ITALIC )
		{
			offset += ITALIC_OFFSET;
		}

		auto it = utfKey.find( c + offset );
		if( it == utfKey.end() ) // char non present dans notre police, que faire ?
		{
			Xoffset += 0.1;
			std::cerr<<" character "<<sentence[i]<<" (utf "<<c<<", char "<<(char)c<<" ) not present in font"<<endl;
			continue;
		}
		int CharIndex = it->second;

		CharacterFont& cF = characterMap[CharIndex];

		float yoff = cF.yoffset * currentModifier.size;
		float xoff = cF.xoffset * currentModifier.size;

		 // get the kerning stuff :
		 //if( face->face_flags &  FT_KERNING_DEFAULT  )
		 if( FT_HAS_KERNING(face) && i > 0 )
		 {
			 FT_Vector akerning;


			 FT_UInt prev_glyph_index = FT_Get_Char_Index( face, sentence[i-1] );
			 FT_UInt curr_glyph_index = FT_Get_Char_Index( face, c );

			 FT_Error error = FT_Get_Kerning( face, (int)sentence[i-1], c, FT_KERNING_DEFAULT, &akerning );
			 if( error )
			 {
				 cout<<"error kerning"<<endl;
			 }
			// cout<<"KERNING : "<<akerning.x<<","<<akerning.y<<endl;

			 float units = (float) face->units_per_EM * 3.0;
			 Xoffset -= (currentModifier.size*akerning.x) / units;
			// assert( akerning.x == 0 );
		 }

		vertices[nbCharacters*4]   = vec3( xoff+  Xoffset,      yoff- Yoffset ,          0.0f   );
		vertices[nbCharacters*4+1] = vec3( xoff+  Xoffset+cF.w * currentModifier.size, yoff- Yoffset  ,         0.0f  );
		vertices[nbCharacters*4+2] = vec3( xoff+  Xoffset+cF.w * currentModifier.size, yoff- Yoffset  + cF.h  * currentModifier.size , 0.0f  );
		vertices[nbCharacters*4+3] = vec3( xoff+  Xoffset,      yoff- Yoffset  + cF.h  * currentModifier.size , 0.0f  );

		texCoord[nbCharacters*4]   = vec2( cF.bitmap_x,        cF.bitmap_y + cF.bitmap_h  );
		texCoord[nbCharacters*4+1] = vec2( cF.bitmap_x + cF.bitmap_w, cF.bitmap_y + cF.bitmap_h  );
		texCoord[nbCharacters*4+2] = vec2( cF.bitmap_x + cF.bitmap_w, cF.bitmap_y   );
		texCoord[nbCharacters*4+3] = vec2( cF.bitmap_x,        cF.bitmap_y   );

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

		Xoffset += cF.xadvance  * currentModifier.size;
/*		if( cF.xadvance * currentModifier.size + Xoffset >= 1.0f )
		{
			Yoffset += 0.1 + 0.05; // size + interline
			Xoffset = 0.0f;
		}
*/
		nbCharacters++;
	}
	sizeY = Yoffset + modifierStack.top().size + interline;
	sizeX = Xoffset;

	// create the letters:
	//cout<<vertices.size()<<" letters in the string "<<endl;//\""<<sentence<<"\""<<endl;
	DynamicMesh* lettersMesh = NULL;
	lettersMesh = new DynamicMesh(nbCharacters*4,nbCharacters*2);
	lettersMesh->updateGeometry( vertices, texCoord, nbCharacters*4, triangles, nbCharacters*2 );
	lettersMesh->updateBuffer( colorBuffer, nbCharacters*4, lettersMesh->mesh.colorBuffer, true );
	return lettersMesh;



}
