// un uniform d'un shader

#ifndef SHADER_UNIFORM_H
#define SHADER_UNIFORM_H

#include "Cleanable.h"
#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>

#endif
#include <glm/glm.hpp>

class Uniform : public ICleanable
{
public:
	Uniform(GLint location, ICleanableObserver* observer);
	virtual void Clean() = 0;
public:
	GLint _location;
	bool _dirty; 
	ICleanableObserver* _observer; // le shader
};

class UniformFloat : public Uniform
{
public:
	UniformFloat(GLint location, ICleanableObserver* observer);

	float _value;

	float getValue();
	void setValue(float v);

	void Clean();
};

class UniformVec2f : public Uniform
{
public:
	UniformVec2f(GLint location, ICleanableObserver* observer);

	glm::vec2 _value;

	glm::vec2 getValue();
	void setValue(const glm::vec2& v);

	void Clean();
};

class UniformVec3f : public Uniform
{
public:
	UniformVec3f(GLint location, ICleanableObserver* observer);

	glm::vec3 _value;

	glm::vec3 getValue();
	void setValue(const glm::vec3& v);

	void Clean();
};

class UniformVec4f : public Uniform
{
public:
	UniformVec4f(GLint location, ICleanableObserver* observer);

	glm::vec4 _value;

	glm::vec4 getValue();
	void setValue(const glm::vec4& v);

	void Clean();
};

class UniformIVec4 : public Uniform
{
public:
	UniformIVec4(GLint location, ICleanableObserver* observer);

	glm::ivec4 _value;

	glm::ivec4 getValue();
	void setValue(const glm::ivec4& v);

	void Clean();
};

class UniformSampler2D : public Uniform
{
public:
	UniformSampler2D(GLint location, int texUnit, ICleanableObserver* observer );

	GLuint _textureID;
	GLint _texUnit;

	GLuint getValue();
	void setValue(const GLuint& v);

	void Clean();
};

#ifndef __ANDROID__ 

class UniformSampler3D : public Uniform
{
public:
	UniformSampler3D(GLint location, int texUnit, ICleanableObserver* observer );

	GLuint _textureID;
	GLint _texUnit;

	GLuint getValue();
	void setValue(const GLuint& v);

	void Clean();
};

class UniformSampler2DArray : public Uniform
{
public:
	UniformSampler2DArray(GLint location, int texUnit, ICleanableObserver* observer );

	GLuint _textureID;
	int _texUnit;

	GLuint getValue();
	void setValue(const GLuint& v);

	void Clean();
};

#endif

class UniformSamplerCube : public Uniform
{
public:
	UniformSamplerCube( GLint location, int texUnit, ICleanableObserver* observer );
	
	GLuint _textureID;
	int _texUnit;

	GLuint getValue();
	void setValue(const GLuint& v);

	void Clean();
};

class UniformMat4f : public Uniform
{
public:
	UniformMat4f(GLint location, ICleanableObserver* observer);

	glm::mat4 _value;
	int _nbValues; // in case of array of matrices

	glm::mat4& getValue();
	void setValue(const glm::mat4& v );
	//void setValue(const float* v, int nbValues );

	void Clean();
};

class UniformMat3f : public Uniform
{
public:
	UniformMat3f(GLint location, ICleanableObserver* observer);

	glm::mat3 _value;

	glm::mat3& getValue();
	void setValue(const glm::mat3& v);

	void Clean();
};

class UniformBoolean : public Uniform
{
public:
	UniformBoolean(GLint location, ICleanableObserver* observer);

	bool _value;

	bool getValue();
	void setValue(bool v);

	void Clean();
};


#endif
