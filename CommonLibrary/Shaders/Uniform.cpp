#include "Uniform.h"
using namespace glm;

#include "CommonLibrary.h"


Uniform::Uniform(GLint location, ICleanableObserver* observer) : _location(location), _observer(observer)
{
	_dirty = true;
	_observer->NotifyDirty(this);
};

UniformFloat::UniformFloat(GLint location, ICleanableObserver* observer) : Uniform(location,observer)
{
	_value = 0.0f;
}

float UniformFloat::getValue()
{
	return _value;
}

void UniformFloat::setValue(float v)
{
	if( _value != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_value = v;
}

void UniformFloat::Clean()
{
	glUniform1f( _location, _value);
	_dirty = false;
}

/***********************************************************************************/

UniformVec2f::UniformVec2f(GLint location, ICleanableObserver* observer) : Uniform(location,observer)
{
	_value = vec2(0,0);
}

glm::vec2 UniformVec2f::getValue()
{
	return _value;
}

void UniformVec2f::setValue(const glm::vec2& v)
{
	if( _value != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_value = v;
}

void UniformVec2f::Clean()
{
	glUniform2f( _location, _value.x, _value.y );
	_dirty = false;
}

/***********************************************************************************/

UniformVec3f::UniformVec3f(GLint location, ICleanableObserver* observer) : Uniform(location,observer)
{
	_value = vec3(0,0,0);
}

glm::vec3 UniformVec3f::getValue()
{
	return _value;
}

void UniformVec3f::setValue(const glm::vec3& v)
{
	if( _value != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_value = v;
}

void UniformVec3f::Clean()
{
	glUniform3f( _location, _value.x, _value.y, _value.z);
	_dirty = false;
}

/*******************************************************************************************/

UniformVec4f::UniformVec4f(GLint location, ICleanableObserver* observer) : Uniform(location,observer)
{
	_value = vec4(0,0,0,0);
}

vec4 UniformVec4f::getValue()
{
	return _value;
}

void UniformVec4f::setValue(const vec4& v)
{
	if( _value != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_value = v;
}

void UniformVec4f::Clean()
{
	glUniform4f( _location, _value.x, _value.y, _value.z, _value.w );
	_dirty = false;
}

/*******************************************************************************************/


UniformIVec4::UniformIVec4(GLint location, ICleanableObserver* observer) : Uniform(location,observer)
{
}

glm::ivec4 UniformIVec4::getValue()
{
	return _value;
}

void UniformIVec4::setValue(const glm::ivec4& v)
{
	if( _value != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_value = v;
}

void UniformIVec4::Clean()
{
	glUniform4i( _location, _value.x, _value.y, _value.z, _value.w );
	_dirty = false;
}

/*******************************************************************************************/

// pour bind des textures
UniformSampler2D::UniformSampler2D(GLint location, GLint texUnit, ICleanableObserver* observer ) : Uniform(location,observer), _texUnit(texUnit)
{
	_textureID = 0;
}

GLuint UniformSampler2D::getValue()
{
	return _textureID;
}

void UniformSampler2D::setValue(const GLuint& v)
{
	if( _textureID != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_textureID = v;
}

void UniformSampler2D::Clean()
{
	#ifndef NDEBUG
	GLenum errCode;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SimpleTextureShader::CLEAN %d",errCode);
	}
	#endif

	glActiveTexture(GL_TEXTURE0+_texUnit);
	glBindTexture(GL_TEXTURE_2D,_textureID);
	glUniform1i(_location,_texUnit);

	#ifndef NDEBUG
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SimpleTextureShader::CLEAN %d",errCode);
	}
	#endif

	_dirty = false;
	_textureID = 0; // we should do this every frame ?
}
/*******************************************************************************************/

// Texture 3D

#ifndef __ANDROID__ 
UniformSampler3D::UniformSampler3D(GLint location, GLint texUnit, ICleanableObserver* observer ) : Uniform(location,observer), _texUnit(texUnit)
{
	_textureID = 0;
}

GLuint UniformSampler3D::getValue()
{
	return _textureID;
}

void UniformSampler3D::setValue(const GLuint& v)
{
	if( _textureID != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_textureID = v;
}

void UniformSampler3D::Clean()
{
	#ifndef NDEBUG
	GLenum errCode;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SimpleTextureShader::CLEAN %d",errCode);
	}
	#endif

	glActiveTexture(GL_TEXTURE0+_texUnit);
	glBindTexture(GL_TEXTURE_3D,_textureID);
	glUniform1i(_location,_texUnit);

	#ifndef NDEBUG
	while ((errCode = glGetError()) != GL_NO_ERROR) {
	  LOGT("DRAW","Error SimpleTextureShader::CLEAN %d",errCode);
	}
	#endif

	_dirty = false;
	_textureID = 0; // we should do this every frame ?
}
#endif

/*********************************************************************************************/

#ifndef __ANDROID__ 

// pour bind plusieurs textures a la fois
UniformSampler2DArray::UniformSampler2DArray(GLint location, GLint texUnit, ICleanableObserver* observer ) : Uniform(location,observer), _texUnit(texUnit)
{
	_textureID = 0;
}

GLuint UniformSampler2DArray::getValue()
{
	return _textureID;
}

void UniformSampler2DArray::setValue(const GLuint& v)
{
	if( _textureID != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_textureID = v;
}

void UniformSampler2DArray::Clean()
{
#ifndef __ANDROID__
	glActiveTexture(GL_TEXTURE0+_texUnit);
	glBindTexture(GL_TEXTURE_2D_ARRAY,_textureID);
	glUniform1i(_location,_texUnit);
#endif

	_dirty = true; //false;
//	_textureID = 0; // we should do this every frame ?
}
#endif


/*********************************************************************************************/
// pour bind des textures
UniformSamplerCube::UniformSamplerCube(GLint location, GLint texUnit, ICleanableObserver* observer ) : Uniform(location,observer), _texUnit(texUnit)
{
	_textureID = 0;
}

GLuint UniformSamplerCube::getValue()
{
	return _textureID;
}

void UniformSamplerCube::setValue(const GLuint& v)
{
	if( _textureID != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_textureID = v;
}

void UniformSamplerCube::Clean()
{
	glActiveTexture(GL_TEXTURE0+_texUnit); 
	glBindTexture(GL_TEXTURE_CUBE_MAP,_textureID);
	glUniform1i(_location,_texUnit); 

	_dirty = false; 
	_textureID = 0; // we should do this every frame ?
}

/*********************************************************************************************/

UniformMat4f::UniformMat4f(GLint location, ICleanableObserver* observer ) : Uniform(location,observer)
{
	_value = glm::mat4(1.0);
}

glm::mat4& UniformMat4f::getValue()
{
	return _value;
}

void UniformMat4f::setValue(const glm::mat4& v)
{
	if( _value != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_value = v;
	_nbValues = 1;
}

void UniformMat4f::setValuePtr(const glm::mat4* v)
{
	setValue(*v);
}

void UniformMat4f::Clean()
{
	glUniformMatrix4fv(_location, 1, false, &_value[0][0] );	
	_dirty = false;
}

/*********************************************************************************************/

UniformMat3f::UniformMat3f(GLint location, ICleanableObserver* observer ) : Uniform(location,observer)
{
	_value = glm::mat3(1.0);
}

glm::mat3& UniformMat3f::getValue()
{
	return _value;
}

void UniformMat3f::setValue(const glm::mat3& v)
{
	if( _value != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}

	_value = v;
}

void UniformMat3f::Clean()
{
	glUniformMatrix3fv(_location, 1, false, &_value[0][0] );	
	_dirty = false;
}

/*********************************************************************************************/

UniformBoolean::UniformBoolean(GLint location, ICleanableObserver* observer): Uniform(location,observer)
{
	_value = false;
}

bool UniformBoolean::getValue()
{
	return _value;
}
void UniformBoolean::setValue(bool v)
{
	if( _value != v && !_dirty)
	{
		_dirty = true;
		_observer->NotifyDirty(this);
	}
	_value = v;
}

void UniformBoolean::Clean()
{
	glUniform1i(_location,_value);
	_dirty = false;
}
