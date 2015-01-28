#include "DepthReader.h"

#include "assert.h"
#include <iostream>
using namespace std;
#include "TextureGL.h"
#include <stdio.h>

// http://www.songho.ca/opengl/gl_pbo.html
// c'est pas simple !

using namespace glm;

#ifdef __ANDROID__

DepthReader::DepthReader()
{
}

float DepthReader::readPixels( const vec2& cursorPos )
{
}

#else

DepthReader::DepthReader()
{
	width = 1024; // on va y aller mollo
	height = 768;

	glGenTextures(1, &depth_buffer);
	glBindTexture(GL_TEXTURE_2D, depth_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, width, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glGenTextures(1, &color_buffer);
	glBindTexture(GL_TEXTURE_2D, color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, width, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glBindTexture(GL_TEXTURE_2D, 0);

	fbo = new FramebufferObject();
	fbo->AttachTexture(GL_TEXTURE_2D, color_buffer, GL_COLOR_ATTACHMENT0);
	fbo->AttachTexture(GL_TEXTURE_2D, depth_buffer, GL_DEPTH_ATTACHMENT );
	fbo->Disable();

	assert( fbo->IsValid() );


	glGenBuffers( 1, &pbo );
	// pack : read from FBO to PBO
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo );
	glBufferData(GL_PIXEL_PACK_BUFFER, sizeof(float)*3*width*width, NULL, GL_STREAM_READ );
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	data = new unsigned char[3*width*width];
}

float DepthReader::readPixels( const vec2& cursorPos )
{
	float depth = -1.0f;

	fbo->Bind();
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo );
	//glBindTexture(GL_TEXTURE_2D, color_buffer );
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	vec2 imagePos = vec2( cursorPos.x, (768.0f-cursorPos.y) );
	//glReadPixels(0,0,512,512,GL_RED,GL_FLOAT,0); // on peut essayer ca : GL_DEPTH_COMPONENT
	glReadPixels(imagePos.x,imagePos.y,1,1,GL_RED,GL_FLOAT,0); // on peut essayer ca : GL_DEPTH_COMPONENT

	GLfloat* ptr = (GLfloat*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY); // we want to read data from the PBO

	if(ptr)
	{
	   // int x = imagePos.x;
	   //  int y = imagePos.y;
	   //  float d = (float)ptr[ x + y* width];
	   float d = ptr[0];
/*	   float minZ = 2.0;
	   float maxZ = 500.0f;
	   depth = (1.0-d)*minZ + (d)*maxZ;
	   cout<<"depth en "<<imagePos.x<<","<<imagePos.y<<" = "<<depth<<endl;

*/
	    float zNear = 20.0f;
	    float zFar  = 200.0f;
	    float z_b = d;
	    float z_n = 2.0 * z_b - 1.0;
	    float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
	    depth = z_e;
	    cout<<"depth en "<<imagePos.x<<","<<imagePos.y<<" = "<<depth<<endl;


	   //cout<<"depth = "<<depth<<endl;

		/*
		 * 	   // http://www.david-amador.com/2012/09/how-to-take-screenshot-in-opengl/
		 *
	   FILE *filePtr = fopen(DATA_PATH "test.tga", "wb");
	   if (!filePtr) return;

	   unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
	   unsigned char header[6] = { 512%256,512/256,
			   	   	   	   	   	   512%256,512/256,
			   	   	   	   	   	   24,0};
	   // We write the headers
	   fwrite(TGAheader,	sizeof(unsigned char),	12,	filePtr);
	   fwrite(header,	sizeof(unsigned char),	6,	filePtr);
	   // And finally our image data
	   fwrite(ptr,	sizeof(GL_UNSIGNED_BYTE), 3*512*512, filePtr);
	   fclose(filePtr);
	   */
	   glUnmapBufferARB(GL_PIXEL_PACK_BUFFER);
	}

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	fbo->Disable();

	return depth;
}

#endif
