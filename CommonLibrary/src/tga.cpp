
#include <stdio.h>
#include <iostream>
#include "assert.h"
using namespace std;
#include "fichiers.h"
#include "stb_image.h"

#include "CommonLibrary.h"

struct HEADER_TGA  
{
   char  idlength;
   char  colourmaptype;
   char  datatypecode;
   short int colourmaporigin;
   short int colourmaplength;
   char  colourmapdepth;
   short int x_origin;
   short int y_origin;
   short width;
   short height;
   char  bitsperpixel;
   char  imagedescriptor;
   //   descriptor: 00vhaaaa
   //   h horizontal flip
   //   v vertical flip
   //   a alpha bits



   HEADER_TGA( char _datatype, short int w, short int h, char _bitsperpixel)
   {
	   idlength = 0; colourmaptype = 0; 
	   datatypecode = _datatype;
	   colourmaporigin = 0;
	   colourmaplength = 0;
	   colourmapdepth = 0;
	   x_origin = 0;
	   y_origin = 0;
	   width = w; height = h;
	   bitsperpixel = _bitsperpixel;
	   imagedescriptor = 0;
   }
};

// http://local.wasp.uwa.edu.au/~pbourke/dataformats/tga/
void saveTGA( char* filename )
{
	int sx = 640;
	int sy = 480;

	FILE* f = fopen(filename, "wb" );
	if( !f )
	{
		cout<<"can't open "<<filename<<" for writing"<<endl;
		return;
	}
	HEADER_TGA h(2,sx,sy,24);
	
	fwrite( &h.idlength, 1, 1, f);
	fwrite( &h.colourmaptype, 1, 1, f);
	fwrite( &h.datatypecode, 1, 1, f);
	fwrite( &h.colourmaporigin, 2, 1, f);
	fwrite( &h.colourmaplength, 2, 1, f);
	fwrite( &h.colourmapdepth, 1, 1, f);
	fwrite( &h.x_origin, 2, 1, f);
	fwrite( &h.y_origin, 2, 1, f);
	fwrite( &h.width, 2, 1, f);
	fwrite( &h.height, 2, 1, f);
	fwrite( &h.bitsperpixel, 1, 1, f);
	fwrite( &h.imagedescriptor, 1, 1, f);

	// les pixels :
	char* data = new char[sx*sy*3];
	for( int i=0; i<sx*sy*3; i+=3 )
	{
		data[i]   = 255; // il faut enregistrer en BGR
		data[i+1] = 255;
		data[i+2] = 255;
	}

	fwrite( data, sizeof(char), sx*sy*3, f);
	
	fclose(f);
	delete[] data;
}

unsigned char* readTGA(const char* filename, int& width, int& height, int& bpp, bool& hasAlpha, bool unflip /* = true*/)
{
	LOGE("loading texture %s", filename);

	unsigned char* data = NULL; // image data parsed
#ifdef __ANDROID__
	AAssetDir* assetDir = AAssetManager_openDir(MyAssetManager::get()->mgr, "");
	const char* dirfilename = (const char*)NULL;

	unsigned char* buffer = NULL; // raw data
	long size = 0;
	while ((dirfilename = AAssetDir_getNextFileName(assetDir)) != NULL)
	{
		if( strcmp(filename, dirfilename) != 0 )
			continue;

	    AAsset* asset = AAssetManager_open(MyAssetManager::get()->mgr, dirfilename, AASSET_MODE_UNKNOWN);
	    if( asset == NULL )
	    {
	    	LOGE("Can't find asset");
	    	continue;
	    }
	    size = AAsset_getLength(asset);
	    buffer = new unsigned char[size];
	    AAsset_read (asset,buffer,size);
	    AAsset_close(asset);
	    AAssetDir_close(assetDir);
	    break;
	}

	data = stbi_load_from_memory( buffer, size, &width, &height, &bpp, 4);
	hasAlpha = false;
	bpp = 32;

	if( unflip )
	{
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
	}

#else
	/*
	FILE* f = fopen(filename, "rb" );
	if( f == NULL )
	{
		cout<<"Couldn't find file "<<filename<<endl;
		assert(false);
	}

	data = stbi_load_from_file( f , &width, &height, &bpp, 4);
	*/
	long size = 0;
	unsigned char* content = readBuffer( filename, size );
	data = stbi_load_from_memory( content, size, &width, &height, &bpp, 4 );

	if( data == NULL )
	{
		FILE* f = fopen(filename, "rb" );
		if( f == NULL )
		{
			cout<<"Couldn't find file "<<filename<<endl;
			assert(false);
		}
		data = stbi_load_from_file( f , &width, &height, &bpp, 4);
		fclose(f);
	}

	hasAlpha = false;
	bpp = 32;

	if( unflip )
	{
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
	}
	//fclose(f);

#endif

	return data;
}
