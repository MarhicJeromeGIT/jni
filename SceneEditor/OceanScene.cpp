#include "OceanScene.h"
#define QT_NO_OPENGL_ES_2 true
#include "GL\glew.h"
#include "Shader.h"

//#include <QOpenGLFramebufferObject>
#include <iostream>
using namespace std;
using glm::vec3;
using glm::mat4;
using glm::vec4;
using glm::vec2;

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
#include "SceneManager.h"
#include <random>
#include "Gui/Image.h"
#include "OceanShader.h"
#include "FramebufferObject.h"

const int N = 128;
const int M = N;

void h0tilde( int n, int m, fftw_complex& result );
float waveTime = 0.0f;

std::mt19937 gen;
std::normal_distribution<float> randomGaussian( 0.0f, 1.0f );

// I perform the FFT on that
fftw_complex* in;
double* out;
fftw_plan backward;

fftw_complex *htilde0;
fftw_complex *htilde0conj;

const float Lx = 128;
const float Lz = Lx;

float multiplicateur = 0.12f;

TextureGL* heightData;
TextureGL* normData;


TextureGL* h0tildeTex;
TextureGL* h0tildeconjTex;

//QOpenGLFramebufferObject* fboOcean = NULL;
FramebufferObject* fboOcean2 = NULL;

float uniformRandomVariable() {
	return (float)rand()/RAND_MAX;
}

void gaussianRandomVariable( float& a, float& b) {
	float x1, x2, w;
	do {
	    x1 = 2.f * uniformRandomVariable() - 1.f;
	    x2 = 2.f * uniformRandomVariable() - 1.f;
	    w = x1 * x1 + x2 * x2;
	} while ( w >= 1.f );
	w = sqrt((-2.f * log(w)) / w);
	a = x1 * w;
	b = x2 * w;
}

OceanScene::OceanScene( SceneManager* manager ) : Scene( manager )
{
	drawTextures  = false;
	drawWireframe = false;
	lambda        = -0.77f;

	std::random_device rd;
	gen = std::mt19937(rd());

	htilde0 = new fftw_complex[N*N];
	htilde0conj = new fftw_complex[N*N];
	for( int n =0; n < N; n++ )
	{
		for( int m=0; m< M; m++ )
		{
			fftw_complex h0;
			h0tilde( n, m, h0 );
			htilde0[n*M+m][0] = h0[0];
			htilde0[n*M+m][1] = h0[1];

			fftw_complex h0conj;
			h0tilde( -n, -m, h0conj );
			htilde0conj[n*M+m][0] = h0conj[0];
			htilde0conj[n*M+m][1] = -h0conj[1];
		}
	}

}

void GenerateInitialTextures()
{
	vec4* tildeConj = new vec4[N*N];
	for( int i=0; i<N; i++ )
	{
		for( int j=0; j<N; j++ )
		{
			tildeConj[i*N+j] = vec4( htilde0[i*N+j][0], htilde0[i*N+j][1], htilde0conj[i*N+j][0], htilde0conj[i*N+j][1] );	
		}
	}

	GLuint h0tildeconjTexId;

	glGenTextures(1, &h0tildeconjTexId);
	glBindTexture(GL_TEXTURE_2D, h0tildeconjTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, N, 0, GL_RGBA, GL_FLOAT, tildeConj);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glBindTexture( GL_TEXTURE_2D, 0 );
	h0tildeconjTex = new TextureGL(h0tildeconjTexId);

	delete[] tildeConj;
}

void OceanScene::PrepareGeometryRendering()
{
	fullscreenQuad = new DynamicMesh(4,2);
	vec3 vert[] = { vec3(0.0,0.0,-10), vec3(1.0,0.0,-10), vec3(1.0,1.0,-10), vec3(0.0,1.0,-10) };
	vec2 uv[]   = { vec2(0.0,0.0), vec2(1.0,0.0), vec2(1.0,1.0), vec2(0.0,1.0) };
	unsigned short tri[2*3] = { 0,1,2, 0,2,3 };
	fullscreenQuad->updateGeometry( vert, uv, 4, tri, 2  );

	oceanGeometryShader = new OceanGeometryGeneratorShader();
	oceanGeometryShader->load();

	//fboOcean = new QOpenGLFramebufferObject( N, N, QOpenGLFramebufferObject::NoAttachment );
	
	vec4* col = new vec4[N*N];
	for( int i=0; i<N; i++ )
	{
		for( int j=0; j<N; j++ )
		{
			float a,b,c,d;
			gaussianRandomVariable(a,b);
			gaussianRandomVariable(c,d);
			col[i*N+j] = vec4(a,b,c,d);
		}
	}

	GLuint texindex;
	glGenTextures(1, &texindex );
	glBindTexture(GL_TEXTURE_2D, texindex);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, N, 0, GL_RGBA, GL_FLOAT, col);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glBindTexture( GL_TEXTURE_2D, 0 );
	

	GLuint texindex2;
	glGenTextures(1, &texindex2 );
	glBindTexture(GL_TEXTURE_2D, texindex2);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, N, 0, GL_RGBA, GL_FLOAT, col);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glBindTexture( GL_TEXTURE_2D, 0 );
/*	
	fboOcean->bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texindex2, 0);
	fboOcean->release();
*/

	fboOcean2 = new FramebufferObject();
	fboOcean2->AttachTexture(GL_TEXTURE_2D, texindex, GL_COLOR_ATTACHMENT0);
	fboOcean2->AttachTexture(GL_TEXTURE_2D, texindex2, GL_COLOR_ATTACHMENT1);
	fboOcean2->Disable();

	delete[] col;
	heightData = new TextureGL(texindex);
	normData   = new TextureGL(texindex2);
}

void OceanScene::init()
{
	GLint maxbuffers;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);

	sceneManager->camera_distance = 40.0f;
//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	waves = new DynamicMesh( N*N*4, N*N*2 );

	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N * N);
	out = (double*) fftw_malloc(sizeof(double) * N * N);
	backward = fftw_plan_dft_c2r_2d(N, N, in, out, FFTW_ESTIMATE);

	vec3* vert = new vec3[N*N*4];
	vec2* uv   = new vec2[N*N*4];
	vec3* norm = new vec3[N*N*4];
	unsigned int* tris = new unsigned int[N*N*2*3];
	int index = 0;

	for( int i=0; i< N; i++ )
	{
		for( int j=0; j< N; j++ )
		{
			float x = (i - N / 2.0f) * Lx / N;
			float z = (j - N / 2.0f) * Lz / N;

			vert[i*N+j] = vec3(x,0,z);
			uv[i*N+j]   = vec2( ((float)i)/ ((float)N), ((float)j)/ ((float)N) );
			norm[i*N+j] = vec3(0,1,0);


			if( i != N-1 && j != N-1 )
			{
				tris[index]   = i*N+j;
				tris[index+1] = i*N+j+1;
				tris[index+2] = (i+1)*N+j+1;

				tris[index+3] = i*N+j;
				tris[index+4] = (i+1)*N+j+1;
				tris[index+5] = (i+1)*N+j;

				index += 6;
			}
		}
	}
	 
	waves->updateGeometry( vert, uv, N*N*4, tris, N*N*2 );
	waves->updateBuffer( norm, N*N*4, waves->mesh.normalBuffer );

	delete[] vert;
	delete[] uv;
	delete[] norm;
	delete[] tris;

	QIntTweakable* timeTweakable = new QIntTweakable( "time", [](int i){ waveTime = (float)i/100.0f; }, waveTime, 0.0, 2000 );
	emit tweakableAdded( timeTweakable );

	QIntTweakable* multTweakable = new QIntTweakable( "multiplicateur", [](int i){ multiplicateur = (float)i/100.0f; }, multiplicateur*100, 0.0, 100 );
	emit tweakableAdded( multTweakable );

	QBooleanTweakable* wireframeTweakable = new QBooleanTweakable( "wireframe", [this](bool b){ drawWireframe = b; }, drawWireframe );
	emit tweakableAdded( wireframeTweakable );

	QBooleanTweakable* textureTweakable = new QBooleanTweakable( "textures", [this](bool b){ drawTextures = b; }, drawTextures );
	emit tweakableAdded( textureTweakable );

	QIntTweakable* lambdaTweakable = new QIntTweakable( "lambda", [this](int i){ lambda = (float)i/100.0f; waveMat->setLambda(lambda); }, lambda*100, -100, 100 );
	emit tweakableAdded( lambdaTweakable );

	
	GenerateInitialTextures();
	PrepareGeometryRendering();


	//heightData = TextureManager::get()->createRGBATexture( "heightData", vec4(1.0,rand(),1.0,1.0), N, N );
		
	heightImg = new Image( vec3(0.20,0.75,-10), heightData );
	heightImg->scale = vec3(0.30,0.30,1.0);

	normImg = new Image( vec3(0.20,0.45,-10), normData );
	normImg->scale = vec3(0.30,0.30,1.0);

	Model* skyboxModel = new Model();
	skyboxModel->Load( DATA_PATH "skybox.txt" );
	OpenGLStaticModel* skyboxGL = new OpenGLStaticModel(skyboxModel);
	std::string filename[6] = { DATA_PATH "skybox_texture_right.png",  DATA_PATH "skybox_texture_left.png",
							    DATA_PATH "skybox_texture_top.png",  DATA_PATH "skybox_texture_bottom.png",
								DATA_PATH "skybox_texture_front.png",  DATA_PATH "skybox_texture_back.png" };
	skybox  = new Skybox(skyboxGL, filename);


	waveMat = new MaterialOcean();
 	waveMat->setDiffuseColor( vec4(0.2,0.2,0.8,1.0) );
	TextureGL* tex = TextureManager::get()->createRGBATexture( "create", vec4(0.0,0.0,0.0,1.0) , 128, 128 );
	waveMat->setTexture(tex);
	waveMat->setCubemap( skybox->cubemap );
	waveMat->setDisplacementMap( heightData );
	waveMat->shader->uNormalMapSampler->setValue( normData->getTexId() );
}

void OceanScene::deinit()
{

}

float Philips( int n , int m )
{
	float A = 0.0005f;

	float k0 = 2 * M_PI * ( n - N/2.0f ) / Lx;
	float k1 = 2 * M_PI * ( m - M/2.0f ) / Lz;
	float lenghtK = sqrt( k0*k0 + k1*k1 );
	if (lenghtK < 0.000001) return 0.0;

	vec2 w(0.0f,32.0f); // wind direction
	float L = glm::length(w) * glm::length(w) /9.81;
	float L2 = L * L * 0.001 * 0.001;

	float lenghtK4 = lenghtK * lenghtK * lenghtK * lenghtK;
	float lengthKL2 = ( lenghtK * L ) * ( lenghtK * L );
	float exponent = exp( -1.0f / lengthKL2 ) / lenghtK4;

	vec2 k(k0,k1);
	float kdotw = 0;
	if( glm::length( k ) > 0.001 )
	{
		kdotw = glm::dot( glm::normalize( k ), glm::normalize( w ) );
	}
	float kdotw2  = kdotw * kdotw;

	float a = A * exponent * kdotw2 * exp( -(lenghtK * lenghtK) * L2 );
	
	return a;
}

void h0tilde( int n, int m, fftw_complex& result )
{
	const float a = 1.0f / sqrt(2.0f);
	//fftw_complex randVar = { Epsr[n][m], Epsi[n][m] };
	
	float epsr,epsi;
	gaussianRandomVariable( epsr, epsi );
	fftw_complex randVar = { epsr, epsi };

	float philips = sqrt( Philips(n,m) );

	result[0] = a * philips * randVar[0];
	result[1] = a * philips * randVar[1];

}

// w
float dispersionRelation( int n, int m )
{
	// w2(k) = gk (g gravitational constant)
	// w(k) = sqrt( gk );
	const float g = 9.81;

	float k0 = 2 * M_PI * ( n - N/2.0f ) / Lx;
	float k1 = 2 * M_PI * ( m - N/2.0f ) / Lz;
	
	float lenghtK = sqrt( k0*k0 + k1*k1 );

	float res = sqrt( g * lenghtK );
    return res;
}

void htilde( int n, int m, float time, fftw_complex& result )
{
	fftw_complex h0;
	h0[0] = htilde0[n*M+m][0];
	h0[1] = htilde0[n*M+m][1];
	//h0tilde( n, m, h0 );
	fftw_complex h0conj;
	h0conj[0] = htilde0conj[n*M+m][0];
	h0conj[1] = htilde0conj[n*M+m][1];

	//h0tilde( -n, -m, h0conj );
	//h0conj[1] = -h0conj[1];

	float w = dispersionRelation( n, m );
	float cos1 = cos(  w* time );
	float sin1 = sin(  w* time );
	float cos2 = cos( -w* time );
	float sin2 = sin( -w* time );

	// h0 * c0 + h0conj * c2
	float r1 = h0[0] * cos1 - h0[1] * sin1 + h0conj[0] * cos2 - h0conj[1] * sin2;
	float r2 = h0[0] * sin1 + h0[1] * cos1 + h0conj[0] * sin2 + h0conj[1] * cos2;

	result[0] = r1;
	result[1] = r2;
}

void OceanScene::update(float dt)
{
	waveTime += 0.020f;

/*	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( &(ShaderParams::get()->projectionMatrix[0][0] ) );
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( &(ShaderParams::get()->viewMatrix[0][0] ) );

	vec3 vert[N*N*4];
	vec3 norm[N*N*4];

	for( int i=0; i< N; i++ )
	{
		for( int j=0; j< N; j++ )
		{
			//float h = out[i*N+j];
			float h = 0;
			vec3  normale = vec3(0,0,0);

			for( int n=0; n< N; n++ )
			{
				for( int m=0; m< M; m++ )
				{
					float kx = 2 * M_PI * ( n - N/2.0f ) / Lx;
					float kz = 2 * M_PI * ( m - N/2.0f ) / Lz;
					vec2 k(kx,kz);
 
					vec2 x = vec2((i - N / 2.0f) * Lx / N, (j - N / 2.0f) * Lz / N); // meh.
					float kdotx = glm::dot( k, x );
 
					fftw_complex c = { cos(kdotx), sin(kdotx) };
					fftw_complex result;

					
				
					htilde( n, m, waveTime, result );
					//fftw_complex result = { htilde0[n*M+m][0], htilde0[n*M+m][1] };
					//h0tilde(n,m,result);

					fftw_complex final; // multiplication de nombre complexes
					final[0] = result[0] * c[0] - result[1] * c[1];
					final[1] = result[0] * c[1] + result[1] * c[0];


					normale += vec3(-kx * final[1], 0.0f, -kz * final[1] );



					h += final[0];
				}
			}
			 

			
			float x = (i - N / 2.0f) * Lx / N;
			float z = (j - N / 2.0f) * Lz / N;

			vert[i*N+j] = vec3(x,h,z);			
			norm[i*N+j] = glm::normalize( vec3(0,1,0) - normale );

	//		glBegin(GL_LINES);
	//		glVertex3f( x,h,z );
	//		glVertex3f( x + norm[i*N+j].x, h + norm[i*N+j].y, z + norm[i*N+j].z );
	//		glEnd();
		}
	}
	

	waves->updateBuffer( norm, N*N*4, waves->mesh.normalBuffer );
	//waves->updateBuffer( vert, N*N*4, waves->mesh.vertexBuffer );

*/	

	generateWaveGeometry();
	
	/*
	float col[N*N*4];
	//float col[N*N*4];
	for( int i=0; i <N; i++ )
	{
		for( int j=0; j < N; j++ )
		{
			col[4*(i*N+j)]   = vert[i*N+j].x;
			col[4*(i*N+j)+1] = vert[i*N+j].y;
			col[4*(i*N+j)+2] = vert[i*N+j].z;
			col[4*(i*N+j)+3] = 1.0f;
			
			//gaussianRandomVariable( col[4*(i*N+j)+1], col[4*(i*N+j)+2] );
			//col[i*N+j] = vert[i*N+j].y;
		}
	}
	glBindTexture(GL_TEXTURE_2D, heightData->getTexId());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, N, 0, GL_RGBA, GL_FLOAT, col);
	glBindTexture(GL_TEXTURE_2D, 0 );
	*/
}

void OceanScene::generateWaveGeometry()
{
	fboOcean2->Bind();

	//fboOcean->bind();
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);

	glViewport(0,0,N, N);
	glClearColor(0.0,0.0, 0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.0f,1000.0f);
	ShaderParams::get()->viewMatrix = mat4(1.0);
	ShaderParams::get()->objectMatrix = mat4(1.0);
	
	oceanGeometryShader->enable( *ShaderParams::get() );
	oceanGeometryShader->N->setValue( (float)N );
	oceanGeometryShader->time->setValue( waveTime );
	oceanGeometryShader->h0tildeConj->setValue( h0tildeconjTex->getTexId() );
	oceanGeometryShader->L->setValue( Lx );
	oceanGeometryShader->multiplicateur->setValue( multiplicateur );
	oceanGeometryShader->Draw( &(fullscreenQuad->mesh) );
	oceanGeometryShader->disable();

//	fboOcean->release();
	fboOcean2->Disable();

	glViewport(0,0,ShaderParams::get()->win_x, ShaderParams::get()->win_y);
	glClearColor( 0.2f,0.2f,0.2f,1.0f );

}

void OceanScene::draw()
{
	Renderer::get()->beginFrame();

	float rapport = ShaderParams::get()->win_x / ShaderParams::get()->win_y;
	ShaderParams::get()->projectionMatrix = glm::perspective(70.0f, rapport, 0.1f, 10000.0f);
	ShaderParams::get()->viewMatrix = glm::lookAt( sceneManager->camera->position, vec3(0,0,0), vec3(0,1,0) );

	DrawCall drawcall;

	mat4 objectMat = mat4(1.0);

	drawcall.Pass     = COLOR;
	drawcall.modelMat = objectMat;
	drawcall.viewMat  = ShaderParams::get()->viewMatrix;
	drawcall.projMat  = ShaderParams::get()->projectionMatrix;

	Shader* shader = waveMat->getShader(COLOR);
	if( shader != NULL )
	{
		drawcall.material          = waveMat;
		drawcall.mesh              = &(waves->mesh);
		drawcall.transparencyMode  = TRANSPARENCY_MODE::GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA;
		drawcall.hasTransparency   = false;
		drawcall.disableDepthWrite = false;
		Renderer::get()->draw( drawcall );
	}

	skybox->Draw(COLOR);

	if( drawWireframe )
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
	}
	Renderer::get()->endFrame();
	if( drawWireframe )
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );	
	}

	// 2D Pass
	Renderer::get()->beginFrame();

	ShaderParams::get()->projectionMatrix = glm::ortho(0.0f,1.0f,0.0f,1.0f,0.1f,1000.0f);
	ShaderParams::get()->viewMatrix = mat4(1.0);

	if( drawTextures )
	{
		heightImg->Draw(GUI);
		normImg->Draw(GUI);
	}

	Renderer::get()->endFrame();




}
