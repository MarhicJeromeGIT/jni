#include "PyroclasticNoiseScene.h"
#define QT_NO_OPENGL_ES_2
#include "GL\glew.h"
#include "Shader.h"

#include <iostream>
using namespace std;
using glm::vec3;
using glm::mat4;
using glm::vec4;

#include <QOpenGLFramebufferObject>
#include "AntigravityConfig.h"
#include "Model.h"
#include "ModelImport.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp> // C++ importer interface
#include "OpenGLModel.h"
#include "Camera.h"
#include "QTweakable.h"
#include "Skybox.h"
#include "TextureGL.h"
#include "Animation.h"
#include "SceneManager.h"
#include "FramebufferObject.h"
#include "Gui\Image.h"
#include "GodrayShader.h"
#define BUILD_DATA 1
#include "macros.h"

#include "SimplexNoise.h"
#include "SimplexTextures.h"
#include "fichiers.h"

const int DIMX = 64;
const int DIMY = 64;
const int DIMZ = 64;
GLuint texId;

double zoom = 128.0f;
double octaves = 3;
double offsetX = 0.0;
double offsetY = 0.0;
double offsetZ = 0.0;

QOpenGLFramebufferObject* backCubeFBO;
const float dimension = 512;

// http://www.dreamincode.net/forums/topic/66480-perlin-noise/
DynamicMesh* unitCube = NULL;

inline double findnoise2(double x,double y)
{
 int n=(int)x+(int)y*57;
 n=(n<<13)^n;
 int nn=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
 return 1.0-((double)nn/1073741824.0);
}

inline double interpolate(double a,double b,double x)
{
 double ft=x * 3.1415927;
 double f=(1.0-cos(ft))* 0.5;
 return a*(1.0-f)+b*f;
}

double noise2(double x,double y)
{
 double floorx=(double)((int)x);//This is kinda a cheap way to floor a double integer.
 double floory=(double)((int)y);
 double s,t,u,v;//Integer declaration
 s=findnoise2(floorx,floory); 
 t=findnoise2(floorx+1,floory);
 u=findnoise2(floorx,floory+1);//Get the surrounding pixels to calculate the transition.
 v=findnoise2(floorx+1,floory+1);
 double int1=interpolate(s,t,x-floorx);//Interpolate between the values.
 double int2=interpolate(u,v,x-floorx);//Here we use x-floorx, to get 1st dimension. Don't mind the x-floorx thingie, it's part of the cosine formula.
 return interpolate(int1,int2,y-floory);//Here we use y-floory, to get the 2nd dimension.
}


// Perlin noise :
void noiseOctave( int octaves, double zoom )
{
	unsigned char* data = new unsigned char[DIMX*DIMY*DIMZ];

	double persistence = 0.5;

	for( int k=0; k< DIMZ; k++ )
	{
		for( int i=0; i< DIMX; i++ )
		{
			for( int j=0; j < DIMY; j++ )
			{ 
	/*		double getnoise =0;
			double frequency = 1.0f;
			double amplitude = 1.0f;
			for(unsigned int a=0;a<octaves;a++)//This loops trough the octaves.
			{
				double x = offsetX + ((double)i)/zoom;
				double y = offsetY + ((double)j)/zoom;
				getnoise += noise2(x*frequency,y*frequency)*amplitude;//This uses our perlin noise functions. It calculates all our zoom and frequency and amplitude
				
				frequency *= 2.0;
				amplitude *= persistence;
			}

			int color= (int)((getnoise*128.0)+128.0);//Convert to 0-256 values.
			if(color>255)
			color=255;
			if(color<0)
			color=0;
			  */


			
			double x = offsetX + ((double)i)/((double)DIMX);
			double y = offsetY + ((double)j)/((double)DIMY);

			float res =  octave_noise_3d( octaves, persistence, 1.0f, x, y, ((double)k)/((double)DIMZ) );

			data[k*DIMX*DIMY+i*DIMY+j] = (res*127.0f)+127.0f;
			

			
			// SIMPLE SPHERE
			vec3 center = vec3( DIMX/2, DIMY/2, DIMZ/2 );
			vec3 thisPoint = vec3(i,j,k);
			float distance = glm::length( thisPoint - center );

			if( distance > 30 )
			{
			//	data[k*DIMX*DIMY+i*DIMY+j] = 0;
			}
			else
			{
				data[k*DIMX*DIMY+i*DIMY+j] += 50;
			}
			

			}
		}
	}

/*
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, DIMX, DIMY, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glBindTexture( GL_TEXTURE_2D, 0 );
*/

    glBindTexture(GL_TEXTURE_3D, texId);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLint mode = GL_REPEAT; // GL_CLAMP_TO_BORDER;
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, mode);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, mode);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, mode);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, DIMX, DIMY, DIMZ, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);

	delete[] data;
}


// UV shaders
class UVShader : public Shader
{
public:
	int vertexAttribLoc;
	int texCoordAttribLoc;

public:
	UVShader() : Shader()
	{
		load();
	}
	void load()
	{	
		LOGT("SHADER","UVShader::load");

		string vs = readFile( SHADER_PATH "UVShader.vs" );
		string ps = readFile( SHADER_PATH "UVShader.ps" );
		LoadShaderFromFile(vs,ps);

		uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
		assert(uModelMat != 0);

		uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
		assert(uViewMat != 0);

		uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
		assert(uProjectionMat != 0);

		vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
		texCoordAttribLoc = glGetAttribLocation( getProgram(), "texCoord" );
	}
	void enable( ShaderParams& params )
	{
		Shader::enable( params );
		glEnableVertexAttribArray(vertexAttribLoc);
		glEnableVertexAttribArray(texCoordAttribLoc);
	}
	void disable()
	{
		glDisableVertexAttribArray(vertexAttribLoc);
		glDisableVertexAttribArray(texCoordAttribLoc);
		Shader::disable();
	}
	void Draw(Mesh* m)
	{
		InternalMesh* mesh = (InternalMesh*) m;
		Clean();

		glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
		glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordArrayBuffer );
		glVertexAttribPointer( texCoordAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
};

// RayCast shader
class RaycastShader : public Shader
{
public:
	int vertexAttribLoc;
	int texCoordAttribLoc;

	UniformSampler2D* backCubeSampler;
	UniformSampler3D* volumetricDataSampler;

public:
	RaycastShader() : Shader()
	{
		load();
	}
	void load()
	{	
		LOGT("SHADER","RaycastShader::load");

		string vs = readFile( SHADER_PATH "RaycastShader.vs" );
		string ps = readFile( SHADER_PATH "RaycastShader.ps" );
		LoadShaderFromFile(vs,ps);

		uModelMat = (UniformMat4f*) GetUniformByName("ModelMatrix");
		assert(uModelMat != 0);

		uViewMat = (UniformMat4f*) GetUniformByName("ViewMatrix");
		assert(uViewMat != 0);

		uProjectionMat = (UniformMat4f*) GetUniformByName("ProjectionMatrix");
		assert(uProjectionMat != 0);

		backCubeSampler = (UniformSampler2D*) GetUniformByName("backCubeSampler");
		assert( backCubeSampler != 0 );

		volumetricDataSampler = (UniformSampler3D*) GetUniformByName("volumetricData");
		assert( volumetricDataSampler != 0 );

		vertexAttribLoc   = glGetAttribLocation( getProgram(), "vertexPosition" );
		texCoordAttribLoc = glGetAttribLocation( getProgram(), "texCoord" );
	}
	void enable( ShaderParams& params )
	{
		Shader::enable( params );
		glEnableVertexAttribArray(vertexAttribLoc);
		glEnableVertexAttribArray(texCoordAttribLoc);
	}
	void disable()
	{
		glDisableVertexAttribArray(vertexAttribLoc);
		glDisableVertexAttribArray(texCoordAttribLoc);
		Shader::disable();
	}
	void Draw(Mesh* m)
	{
		InternalMesh* mesh = (InternalMesh*) m;
		Clean();

		glBindBuffer( GL_ARRAY_BUFFER, mesh->vertexBuffer );
		glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer( GL_ARRAY_BUFFER, mesh->texCoordArrayBuffer );
		glVertexAttribPointer( texCoordAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBuffer );
		glDrawElements( GL_TRIANGLES, mesh->nbTriangles * 3, GL_UNSIGNED_SHORT, 0 );
	}
};




PyroclasticNoiseScene::PyroclasticNoiseScene(SceneManager* manager) : Scene(manager)
{
}

void PyroclasticNoiseScene::init()
{
	sceneManager->camera->position = glm::vec3(0,0,10);

	// Skybox
	Model* skyboxModel = new Model();
	skyboxModel->Load( DATA_PATH "skybox.txt" );
	OpenGLStaticModel* skyboxGL = new OpenGLStaticModel(skyboxModel);
	std::string filename[6] = { DATA_PATH "skybox_texture_right.png",  DATA_PATH "skybox_texture_left.png",
							    DATA_PATH "skybox_texture_top.png",  DATA_PATH "skybox_texture_bottom.png",
								DATA_PATH "skybox_texture_front.png",  DATA_PATH "skybox_texture_back.png" };
	skybox  = new Skybox(skyboxGL, filename);

	// Global lighting parameters
	ShaderParams::get()->ambient                       = vec4(0.5,0.5,0.5,1.0);
	ShaderParams::get()->lights[0].lightPosition       = glm::vec4(24,41,-50,1.0);
	ShaderParams::get()->lights[0].lightDiffuseColor   = glm::vec4(1.0,1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightSpecularColor  = glm::vec3(1.0,1.0,1.0);
	ShaderParams::get()->lights[0].lightShininess      = 32;

	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );

	glGenTextures(1, &texId );
	glBindTexture(GL_TEXTURE_2D, texId);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, DIMX, DIMY, 0, GL_LUMINANCE, GL_FLOAT, NULL);
	glBindTexture( GL_TEXTURE_2D, 0 );

	noiseOctave(octaves,zoom);

	unitCube = new DynamicMesh(8,12);
	vec3 vert[8] = { vec3(-0.5,-0.5,-0.5), vec3(0.5,-0.5,-0.5), vec3(0.5,0.5,-0.5), vec3(-0.5,0.5,-0.5),
		             vec3(-0.5,-0.5,0.5), vec3(0.5,-0.5,0.5), vec3(0.5,0.5,0.5), vec3(-0.5,0.5,0.5) };
	vec3 uv[8] = { vec3(0,0,0), vec3(1,0,0), vec3(1,1,0), vec3(0,1,0),
		           vec3(0,0,1), vec3(1,0,1), vec3(1,1,1), vec3(0,1,1) };
	unsigned short tris[12*3] = { 2,1,0,
							      3,2,0,
								  1,6,5,
								  1,2,6,
								  5,7,4,
								  5,6,7,
								  4,3,0,
								  4,7,3,
								  0,5,4,
								  0,1,5,
								  3,7,6,
								  3,6,2 };
	unitCube->updateGeometry( vert, uv, 8, tris, 12 );


	TextureGL* tex = new TextureGL( texId );
	NoiseImage = new Image( vec3(0.15,0.15,0), tex );
	NoiseImage->scale = vec3( 0.3,0.3,1.0 );
	Material3DSlice* mat3D = new Material3DSlice();
	mat3D->setTexture( tex );
	delete NoiseImage->overlayMaterial;
	NoiseImage->overlayMaterial = mat3D;

	QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setMipmap(false);
    fboFormat.setSamples(0);
	fboFormat.setInternalTextureFormat(GL_RGBA32F);
	fboFormat.setAttachment( QOpenGLFramebufferObject::Attachment::Depth );

	// Generate the back facing cube in a texture
	backCubeFBO = new QOpenGLFramebufferObject( dimension, dimension, fboFormat );
	assert( backCubeFBO->isValid() );	
	TextureGL* tex2 = new TextureGL( backCubeFBO->texture() );
	BackCubeImage = new Image( vec3(0.15,0.45,0), tex2 );
	BackCubeImage->scale = vec3( 0.3,0.3,1.0 );

	glEnable( GL_CULL_FACE );
	

	uvshader = new UVShader();
	raycastShader = new RaycastShader();
	raycastShader->backCubeSampler->setValue( backCubeFBO->texture() );

	// Tweakables
	QIntTweakable* octaveTweakable = new QIntTweakable( "octaves",
		[](int o){ 
			octaves = o;
			noiseOctave(octaves,zoom);
		}, octaves, 1, 40 );
	emit tweakableAdded( octaveTweakable );

	QIntTweakable* zoomTweakable = new QIntTweakable( "zoom",
		[](int o){ 
			zoom = ((float)o)/100.0f;
			noiseOctave(octaves,zoom);
	},zoom,0.0f, 10.0f*100.0f );
	emit tweakableAdded( zoomTweakable );

	QIntTweakable* offsetXTweakable = new QIntTweakable( "offset X",
		[](int o){ 
			offsetX = ((float)o)/1000.0f;
			noiseOctave(octaves,zoom);
	},offsetX, 0.0f, 1000.0f*100.0f );
	emit tweakableAdded( offsetXTweakable );
	
	QIntTweakable* offsetYTweakable = new QIntTweakable( "offset Y",
		[](int o){ 
			offsetY = ((float)o)/1000.0f;
			noiseOctave(octaves,zoom);
	},offsetY, 0.0f, 1000.0f*100.0f );
	emit tweakableAdded( offsetYTweakable );
	
	QIntTweakable* offsetZTweakable = new QIntTweakable( "offset Z",
		[](int o){ 
			offsetZ = ((float)o)/100.0f;
			noiseOctave(octaves,zoom);
	},offsetZ, 0.0f, 1.0f*100.0f ); 
	emit tweakableAdded( offsetZTweakable );

}

void PyroclasticNoiseScene::deinit()
{

}

void PyroclasticNoiseScene::update(float dt)
{
}

void PyroclasticNoiseScene::drawCube()
{
	// Back facing
	backCubeFBO->bind();
	glCullFace( GL_FRONT );
	glViewport(0,0,dimension, dimension);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 10000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );
	ShaderParams::get()->objectMatrix = mat4(1.0);
	
	uvshader->enable( *ShaderParams::get() );
	uvshader->Draw( &(unitCube->mesh) );
	uvshader->disable();

	backCubeFBO->release();
	glViewport(0,0,ShaderParams::get()->win_x, ShaderParams::get()->win_y);
	
	glCullFace( GL_BACK );
	
	// RAYCASTING 
	raycastShader->enable( *ShaderParams::get() );
	raycastShader->backCubeSampler->setValue( backCubeFBO->texture() );
	raycastShader->volumetricDataSampler->setValue( texId );
	raycastShader->Draw( &(unitCube->mesh) );
	raycastShader->disable();
}

void PyroclasticNoiseScene::draw()
{
	drawCube();
	
	// 2D Pass
	Renderer::get()->beginFrame();

	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.1f,1000.0f);
	ShaderParams::get()->viewMatrix = mat4(1.0);

	BackCubeImage->Draw(GUI);
	NoiseImage->Draw(GUI);

	Renderer::get()->endFrame();

	
}


