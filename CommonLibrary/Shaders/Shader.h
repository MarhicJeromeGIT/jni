#pragma once

#include "CommonLibrary.h"
#include "Cleanable.h"
#include <string>
using namespace std;

/*
#ifdef __ANDROID__
	#include <stl/config/_android.h>
#endif
*/

#include <map>
class Uniform;
class UniformMat4f;
class Mesh;

#include "ShaderParams.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

enum SHADER_TYPE
{
	BUMP_SHADER,
	BUMP_SPECULAR_MAP_SHADER,
	BUMP_MIX_SPECULAR_MAP_SHADER, // fouillouillou
	CEL_SHADER,
	CUBEMAP_REFLECTION_SHADER,
	CUBEMAP_REFLECTION_TEXTURE_SHADER,
	CUBEMAP_REFLECTION_TEXTURE_NORMAL_SHADER,
	COLOR_TEXT_SHADER,
	DEFERRED_SHADER,
	DEFERRED_SHADER_FULLSCREEN,
	DEFFERED_SHADER_LIGHT_COMBINE,
	DIFFUSE_SHADER,
	DIFFUSE_SHADOW_SHADER,
	DISCO_BALL_SHADER,
	EXPLOSION_WAVE_SHADER,
	FIRE_SHADER,
	FLAMETHROW_SHADER,
	FLAMETHROW_SIMPLE_SHADER,
	GAUSSIAN_BLUR_SHADER,
	GHOST_SHADER,
	GLASS_SHADER,
	GRASS_SHADER,
	GRASS_SHADOW_SHADER,
	INSTANCING_STATIC_SHADER,
	LASER_SHADER,
	LASER_SIMPLE_SHADER,
	LINE_SHADER,
	MORPH_SHADER,
	MULTI_TEXTURE_SHADER,
	OCEAN_SHADER,
	PARTICLE_SHADER,
	PHONG_SHADER,
	PHONG_TEXTURE_SHADER,
	PHONG_WITH_DEPTH,
	SHADOW_MAP_GENERATOR_SHADER,
	SIMPLE_TEXTURE_SHADER,
	SHOCKWAVE_SHADER,
	SOBEL_SHADER,
	SKINNING_SHADER,
	SKINNING_CUBEMAP_SHADER,
	SKINNING_SHADOW_SHADER,
	SKINNING_SPECULAR_SHADER,
	SKINNING_SPECULAR_CRAYON_SHADER,
	SKINNING_SPECULAR_DIFFUSE_MIX_SHADER,
	SKYBOX_SHADER,
	TEXTURE_ARRAY_SHADER,
	TEXTURE_MIX_SHADER,
	TEXTURE_SHADER,
	TEXTURE_3D_SLICE,
	TEXTURE_SHADER_VERTEX_TRANSPARENCY,
	WATER_SHADER,

	SHADER_COUNT,
};

class Shader;
class ShaderManager
{
private:
	ShaderManager();
	static ShaderManager* thisObject;

	Shader* shaders[SHADER_COUNT];

public:
	static ShaderManager* get();
	Shader* getShader( SHADER_TYPE );

	void setShaderPath(const std::string& path);
	void reloadShaders();
};

class Shader : public ICleanableObserver
{
protected:
	static GLuint currentProgram;
	
	vector<ICleanable*> _dirtyUniform;
	map<string,Uniform*> _uniforms;


protected:
	GLuint _program;
	GLuint vertexShader;
	GLuint fragmentShader;

	bool LoadShaderFromFile( const string& vertexSource, const string& fragmentSource );
	bool LoadShaderFromFile( const string& vertexSource, const string&  geometrySource, const string& fragmentSource );
	void CompileProgram( const string& vertexSource, const string& fragmentSource );
	void CompileProgram( const string& vertexSource, const string& geometrySource, const string& fragmentSource );
	void GetUniforms();

	GLuint checkCompileStatus(GLuint shader, GLint *status);


	// Will have to merge this later
	// return true if successful.
	bool newCompileProgram(const string& vertexSource, const string& fragmentSource);
	
	std::string errorString;
public:
	const std::string& getErrorString();
	static std::string shader_path;

	Shader();
	~Shader();

	virtual void load(){};
	virtual void Draw( Mesh* mesh ) = 0;

	Uniform* GetUniformByName(const string& name);

	void Clean();
	void NotifyDirty(ICleanable *value);

	virtual void enable( const ShaderParams& params );
	virtual void disable();

	GLuint getProgram(){ return _program; }

	UniformMat4f*      uModelMat;
	UniformMat4f*      uViewMat;
	UniformMat4f*      uProjectionMat;

private:
	GLuint compileProgram(const string& vertexSource, const string& fragmentSource);
	GLuint compileProgram(const string& vertexSource, const string& gsource, const string& fragmentSource);
};

