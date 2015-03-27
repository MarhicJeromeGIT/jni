#include "Shader.h"

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

#include <iostream>
#include <algorithm>
using namespace std;

#include "CommonLibrary.h"
#include "ShaderParams.h"
#include "Uniform.h"

#include "TextureShader.h"
#include "DiffuseShader.h"
#include "ShadowMapGenerator.h"
#include "PhongShader.h"
#include "FireShader.h"
#include "ParticleShader.h"
#include "SkinnedShader.h"
#include "BumpShader.h"
#include "LaserShader.h"
#include "FlameThrowerShader.h"
#include "WaterShader.h"
#include "GhostShader.h"
#include "MorphShader.h"
#include "DiscoBallShader.h"
#include "TextShader.h"
#include "SkyboxShader.h"
#include "OceanShader.h"
#include "CubemapReflectionShader.h"
#include "CelShader.h"
#include "FullScreenEffectShader.h"
#include "ShockwaveShader.h"
#include "ExplosionWaveShader.h"
#include "GlassShader.h"
#include "GrassShader.h"
#include "InstancingShader.h"
#include "DeferredShader.h"

//**************************************
// Shader manager
//
//**************************************

ShaderManager* ShaderManager::thisObject = NULL;

ShaderManager::ShaderManager()
{
	for( int i=0; i< SHADER_TYPE::SHADER_COUNT; i++ )
	{
		shaders[i] = NULL;
	}

	shaders[BUMP_SHADER]                  = new BumpShader();
	shaders[BUMP_SPECULAR_MAP_SHADER]     = new BumpSpecularMapShader();
	shaders[BUMP_MIX_SPECULAR_MAP_SHADER] = new BumpMixSpecularMapShader();
	shaders[CEL_SHADER]                  = new CelShader();
	shaders[CUBEMAP_REFLECTION_SHADER]   = new CubemapReflectionShader();
	shaders[CUBEMAP_REFLECTION_TEXTURE_SHADER] = new CubemapReflectionShaderWithTexture();
	shaders[CUBEMAP_REFLECTION_TEXTURE_NORMAL_SHADER] = new CubemapReflectionShaderWithTextureAndNormal();
	shaders[COLOR_TEXT_SHADER]           = new ColorTextShader();
	shaders[DEFERRED_SHADER]             = new DeferredPassShader();
	shaders[DEFERRED_SHADER_FULLSCREEN]  = new DeferredShaderFullScreen();
	shaders[DEFFERED_SHADER_LIGHT_COMBINE] = new DeferredShaderLightCombine();
	shaders[DIFFUSE_SHADER]              = new DiffuseShader();
//	shaders[DIFFUSE_SHADOW_SHADER]       = new DiffuseShadowShader();
//	shaders[DISCO_BALL_SHADER]           = new DiscoBallShader();
	shaders[EXPLOSION_WAVE_SHADER]       = new ExplosionWaveShader();
/*	shaders[FIRE_SHADER]                 = new FireShader();
	shaders[FLAMETHROW_SHADER]           = new FlameThrowerShader();
	shaders[FLAMETHROW_SIMPLE_SHADER]    = new FlameThrowerShaderSimple();
*/	shaders[GAUSSIAN_BLUR_SHADER]        = new GaussianBlurShader();
	shaders[GLASS_SHADER]                = new GlassShader();
//  shaders[GHOST_SHADER]                = new GhostShader();
	shaders[GRASS_SHADOW_SHADER]         = new GrassShadowProjectionShader();
	shaders[INSTANCING_STATIC_SHADER]    = new InstancingShader();
	shaders[LASER_SHADER]				 = new LaserShader();
	shaders[LASER_SIMPLE_SHADER]         = new LaserShaderDummyVertex();
	//shaders[LINE_SHADER]                 = new LineShader();
	shaders[OCEAN_SHADER]                = new OceanShader();
	shaders[MORPH_SHADER]				 = new MorphShader();
//	shaders[MULTI_TEXTURE_SHADER]        = new MultiTextureShader(2);
	shaders[PARTICLE_SHADER]             = new ParticleShader();
	shaders[PHONG_SHADER]                = new PhongShader();
	shaders[PHONG_TEXTURE_SHADER]        = new PhongTextureShader();
	shaders[PHONG_WITH_DEPTH]            = new PhongWithDepthShader();
	shaders[SHADOW_MAP_GENERATOR_SHADER] = new ShadowMapGenerator();
	shaders[SHOCKWAVE_SHADER]            = new ShockwaveShader();
	shaders[SIMPLE_TEXTURE_SHADER]		 = new SimpleTextureShader();
	shaders[SKINNING_SHADER]             = new SkinningShader();
	shaders[SKINNING_CUBEMAP_SHADER]     = new SkinningShaderCubemapReflection();
	shaders[SKINNING_SHADOW_SHADER]      = new SkinningShadowMapShader();
	shaders[SKINNING_SPECULAR_SHADER]    = new SkinningSpecularShader();
	shaders[SKINNING_SPECULAR_CRAYON_SHADER] = new SkinningNormalCrayonShader();
	shaders[SKINNING_SPECULAR_DIFFUSE_MIX_SHADER] = new SkinningSpecularMixDiffuseShader();
	shaders[SKYBOX_SHADER]               = new SkyboxShader();
	shaders[SOBEL_SHADER]                = new SobelShader();
	shaders[TEXTURE_SHADER]              = new TextureShader();
	shaders[TEXTURE_SHADER_VERTEX_TRANSPARENCY] = new PerVertexTransparencyTextureShader();
	shaders[TEXTURE_MIX_SHADER]          = new TextureMixShader();
#ifndef __ANDROID__
	shaders[TEXTURE_3D_SLICE]            = new TextureSliceShader();
	shaders[GRASS_SHADER]                = new MovingGrassShader();
#endif
	
//	shaders[WATER_SHADER]          		 = new WaterShader();

/*for( int i=0; i< SHADER_TYPE::SHADER_COUNT; i++ )
{
	if( shaders[i] != NULL )
	{
		shaders[i]->load();
	}
}*/


#ifndef __ANDROID__
//	shaders[TEXTURE_ARRAY_SHADER]        = new TextureArrayShader();
#endif
}

void ShaderManager::setShaderPath(const std::string& path)
{
	Shader::shader_path = path;
}

ShaderManager* ShaderManager::get()
{
	if( thisObject == NULL )
	{
		LOGT("SHADER","Creating new ShaderManager");
		thisObject = new ShaderManager();
		LOGT("SHADER","OK!");
	}
	return thisObject;
}

Shader* ShaderManager::getShader( SHADER_TYPE type )
{
	if(shaders[type] != NULL )
	{
		return shaders[type];
	}

	LOGE("SHADER NON TROUVE %d",type);
	return NULL;
}

void ShaderManager::reloadShaders()
{
	int nbShaders = 0;
	for( int i=0; i< SHADER_TYPE::SHADER_COUNT; i++ )
	{
		if( shaders[i] != NULL )
		{
			shaders[i]->load();
			nbShaders++;
		}
	}

	LOGE("reloaded %d shaders",nbShaders);
}

//**************************************
// Shader base class
//
//**************************************

GLuint Shader::currentProgram = -1;
string Shader::shader_path = "SHADER/PATH/NOT/SET";

Shader::Shader()
{
	_program = -1;
	vertexShader = -1;
	fragmentShader = -1;
	errorString = "no error";
}

void Shader::GetUniforms()
{
	// that may not be empty if we are reloading our shaders :
	for( auto it = _uniforms.begin(); it != _uniforms.end(); it++ )
	{
		delete (*it).second;
	}
	_dirtyUniform.clear();
	_uniforms.clear();

	// liste tous les uniform :
	GLint nbUniforms = 0;
	glGetProgramiv(_program,GL_ACTIVE_UNIFORMS,&nbUniforms);
	//cout<<nbUniforms<<" uniforms dans le shader"<<endl;

	int textureIndex = 0;

	for(int i=0; i< nbUniforms; i++ )
	{
		char uniformName[128];
		GLint size;
		GLenum type;
		glGetActiveUniform(_program,  i,  128,  0,  &size,  &type,  uniformName);

		int uniformLocation = glGetUniformLocation(_program, uniformName);
		if( i != uniformLocation )
		{
			// Ahah ! big thanks to http://www.alecjacobson.com/weblog/?p=2203
			//LOGE(" %d different from %d for uniform %s",i,uniformLocation, uniformName );
			//i = uniformLocation;
		}

		//LOGE("new uniform : %s sampler2D ? %s", uniformName, GL_SAMPLER_2D == type ? "true" : "false");
		//cout<<uniformName<<","<<type<<endl;
		
		switch( type )
		{
			case GL_BOOL:
				{
					Uniform* uniform = new UniformBoolean(uniformLocation,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					break;
				}
			case GL_FLOAT:
				{
					Uniform* uniform = new UniformFloat(uniformLocation,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					break;
				}			
			case GL_FLOAT_VEC2:
				{
					Uniform* uniform = new UniformVec2f(uniformLocation,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					break;
				}
			case GL_FLOAT_VEC3:
				{
					Uniform* uniform = new UniformVec3f(uniformLocation,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					break;
				}
			case GL_FLOAT_VEC4:
				{
					Uniform* uniform = new UniformVec4f(uniformLocation,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					break;
				}

			case GL_INT_VEC4:
				{
					Uniform* uniform = new UniformIVec4(uniformLocation,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					break;
				}

			case GL_SAMPLER_2D:
				{
					Uniform* uniform = new UniformSampler2D(uniformLocation,textureIndex,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					textureIndex++;
					break;
				}

#ifndef __ANDROID__
			case GL_SAMPLER_3D:
				{
					Uniform* uniform = new UniformSampler3D(uniformLocation,textureIndex,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					textureIndex++;
					break;
				}
#endif

#ifndef __ANDROID__
			case GL_SAMPLER_2D_ARRAY:
				{
					Uniform* uniform = new UniformSampler2DArray(uniformLocation,textureIndex,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					textureIndex++;
					break;
				}
#endif

			case GL_SAMPLER_CUBE:
				{
					LOGT("SHADER","GL_SAMPLER_CUBE");

					Uniform* uniform = new UniformSamplerCube(uniformLocation,textureIndex,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					textureIndex++;
					break;
				}

				
			case GL_FLOAT_MAT4:
				{
					Uniform* uniform = new UniformMat4f(uniformLocation,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					break;
				}
			
			case GL_FLOAT_MAT3:
				{
					Uniform* uniform = new UniformMat3f(uniformLocation,this);
					_uniforms.insert( pair<string,Uniform*>(uniformName,uniform) );
					break;
				}
                        
			default:
				LOGE("TYPE NON PRIS EN COMPTE %s",uniformName);
				break;
		}
	}
}

bool Shader::LoadShaderFromFile( const string& vertexSource, const string& fragmentSource )
{
	if( glIsProgram(_program) ) glDeleteProgram(_program);
	if( glIsShader(vertexShader) ) glDeleteShader(vertexShader);
	if( glIsShader(fragmentShader) ) glDeleteShader(fragmentShader);

#ifdef __ANDROID__
	string newFrag = "precision mediump float;\n" + fragmentSource;
	string newVert = "precision mediump float;\n" + vertexSource;
	_program = compileProgram(newVert, newFrag);
#else
	_program = compileProgram(vertexSource, fragmentSource);
#endif
	GetUniforms();
	return true;
}

bool Shader::LoadShaderFromFile( const string& vertexSource, const string&  geometrySource, const string& fragmentSource )
{
	_program = compileProgram(vertexSource, geometrySource, fragmentSource);
	GetUniforms();
	return true;
}

Shader::~Shader()
{
}


void Shader::enable( const ShaderParams& params )
{
	if( true || Shader::currentProgram != _program )
	{
		glUseProgram(_program);
		Shader::currentProgram = _program;
	}
	uModelMat->setValue( params.objectMatrix );
	uViewMat->setValue( params.viewMatrix );
	uProjectionMat->setValue( params.projectionMatrix );
}

void Shader::disable()
{
	// pas forcement besoin
	glUseProgram(0);
}

Uniform* Shader::GetUniformByName(const string& name)
{
	auto it = _uniforms.find(name);
	Uniform* u = NULL;
	if (it != _uniforms.end())
	{
		u = (*it).second;
	}
	else
	{
		LOGE("couldn't find uniform %s", name.c_str());
		for( it = _uniforms.begin(); it != _uniforms.end(); it++ )
		{
			LOGE(" uniform : %s", (*it).first.c_str() );
		}
	}
	return u;
}

void Shader::Clean()
{
	//for_each(_dirtyUniform.begin(), _dirtyUniform.end(), mem_fun(&ICleanable::Clean));
	for( auto it = _dirtyUniform.begin(); it != _dirtyUniform.end(); it++ )
	{
		(*it)->Clean();
	}
	_dirtyUniform.clear();
}

void Shader::NotifyDirty(ICleanable *value)
{
	_dirtyUniform.push_back(value);
}

GLuint Shader::checkCompileStatus(GLuint shader, GLint *status)
{
	int len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 1)
	{
		GLchar* log_string = new char[len + 1];
		glGetShaderInfoLog(shader, len, 0, log_string);
		LOGE("Log found : \n%s", log_string);
		errorString = std::string(log_string);
		delete log_string;
		return 0;
	}

	char log[2048];
    glGetShaderiv(shader, GL_COMPILE_STATUS, status);
    if (!(*status))
    {
         glGetShaderInfoLog(shader, 2048, (GLsizei*)&len, log);
         LOGE("Error: shader %s", log);
         glDeleteShader(shader);
		 errorString = std::string(log);
         return 0;
    }
    return 1;
}

GLuint Shader::compileProgram(const string& vsource, const string& fsource)
{
	GLuint prog = compileProgram( vsource, "", fsource );
	return prog;
}

GLuint Shader::compileProgram(const string& vsource, const string& gsource, const string& fsource)
{
	bool checkStatusAnyway = false;

    vertexShader   = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLint compiled = 0;

	 string vsource2 = string(vsource);
	 while( vsource2[vsource2.size()-1] != '}' )
	 {
		 vsource2.resize (vsource2.size () - 1);
	 }
	 const char* verterChar = vsource2.c_str();
	 glShaderSource(vertexShader, 1, &verterChar, 0);

	 string fsource2 = string(fsource);
	 while( fsource2[fsource2.size()-1] != '}' )
	 {
		 fsource2.resize (fsource2.size () - 1);
	 }
	 const char* fragChar = fsource2.c_str();
	 glShaderSource(fragmentShader, 1, &fragChar, 0);

    glCompileShader(vertexShader);


    if (checkStatusAnyway || checkCompileStatus(vertexShader, &compiled) == 0) {
#ifdef __ANDROID__
    	   LOGE("COMPILE VERTEX PROGRAM FAIL");
#else
       printf("<compileProgram compilation error with vertexShader>:\n");
       printf("%s\n", vsource.c_str());
	   system("pause");
#endif
       //return 0;
    }

    glCompileShader(fragmentShader);

    if (checkStatusAnyway || checkCompileStatus(fragmentShader, &compiled) == 0) {
#ifdef __ANDROID__
    	   LOGE("COMPILE FRAGMENT PROGRAM FAIL");
    	   LOGE("%s",fsource.c_str());
#else
       printf("<compileProgram compilation error with fragmentShader>:\n");
       printf("%s\n", fsource.c_str());
	    system("pause");
       //return 0;
#endif
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

  if (gsource.size() > 0) {
#ifdef __ANDROID__
#else
		const char* geoChar = gsource.c_str();

        GLuint geomShader = glCreateShader(GL_GEOMETRY_SHADER_EXT);
        glShaderSource(geomShader, 1, &geoChar, 0);
        glCompileShader(geomShader);
        glGetShaderiv(geomShader, GL_COMPILE_STATUS, (GLint*)&compiled);
        if (checkStatusAnyway || checkCompileStatus(geomShader, &compiled) == 0) {
           printf("<compileProgram compilation error with geomShader>:\n");
           printf("%s\n", gsource.c_str());
		   system("pause");
           return 0;
        }

        glAttachShader(program, geomShader);

        GLenum gsInput = GL_POINTS;
        GLenum gsOutput = GL_TRIANGLE_STRIP;
        glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT, gsInput);
        glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT, gsOutput);
        glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, 3);
#endif
    }

    glLinkProgram(program);

    // check if program linked
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);



    if (!success) {
        char temp[10000];
        glGetProgramInfoLog(program, 10000, 0, temp);

        LOGE("ERREUR LINKING PROGRAM");
        LOGE("%s", temp);

        glDeleteProgram(program);
		 system("pause");
        program = 0;
        exit(-1);
    }


    return program;
}

const std::string& Shader::getErrorString()
{
	return errorString;
}

bool Shader::newCompileProgram(const string& vertexSource, const string& fragmentSource)
{
	GLuint newVertexShader   = glCreateShader(GL_VERTEX_SHADER);
    GLuint newFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLint compiled = 0;

	 // Remove everything after the closing bracket "}"
	 string vsource2 = string(vertexSource);
	 vsource2.resize( vsource2.find_last_of('}') +1 );
	 const char* verterChar = vsource2.c_str();
	 glShaderSource(newVertexShader, 1, &verterChar, 0);

	 string fsource2 = string(fragmentSource); 
	 fsource2.resize( fsource2.find_last_of('}') +1 );
	 const char* fragChar = fsource2.c_str();
	 glShaderSource(newFragmentShader, 1, &fragChar, 0);

    glCompileShader(newVertexShader);
	int size =0;
	char source[1000];
	glGetShaderSource(newVertexShader, 1000, &size, source);
    if ( checkCompileStatus(newVertexShader, &compiled) == 0) 
	{
       return false;
    }

    glCompileShader(newFragmentShader);
    if ( checkCompileStatus(newFragmentShader, &compiled) == 0) {
		return false;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, newVertexShader);
    glAttachShader(program, newFragmentShader);
    glLinkProgram(program);

    // check if program linked
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char temp[10000];
        glGetProgramInfoLog(program, 10000, 0, temp);
        glDeleteProgram(program);
        program = 0;
		errorString = temp;
		return false;
    }

	vertexShader = newVertexShader;
	fragmentShader = newFragmentShader;
    _program = program;
	return true;
}