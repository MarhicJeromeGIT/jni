#include "Renderer.h"
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include "CommonLibrary.h"
#include "Material.h"

#ifdef __ANDROID__
	#include <jni.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif

//*************************************
// Draw call
//
//*************************************

bool DrawCallComparison( const DrawCall& c1, const DrawCall& c2 )
{
	// order by Pass, then material
	if( c1.Pass != c2.Pass )
	{
		return c1.Pass < c2.Pass;
	}

	// if has transparency, draw it last :
	if( c1.hasTransparency != c2.hasTransparency )
	{
		return( c1.hasTransparency < c2.hasTransparency );
	}

	// compare by z order for GUI drawing :
	if( c1.Pass == GUI )
	{
		float z1 = c1.modelMat[3][2];
		float z2 = c2.modelMat[3][2];

		if( abs(z1-z2) > 0.01f )
		{
			return z1 < z2;
		}
	}

	if( c1.hasTransparency ) // if they are both transparent, sort by distance to the camera
	{
		// if they both have transparency, sort by transparency mode :
		if( c1.transparencyMode != c2.transparencyMode )
		{
			return c1.transparencyMode < c2.transparencyMode;
		}

		// not sure how to do that :/
		/*
		vec3 cam;
		cam.x = c1.viewMat[3][0];
		cam.y = c1.viewMat[3][1];
		cam.z = c1.viewMat[3][2];

		vec3 o1;
		o1.x = c1.modelMat[3][0];
		o1.y = c1.modelMat[3][1];
		o1.z = c1.modelMat[3][2];

		vec3 o2;
		o2.x = c2.modelMat[3][0];
		o2.y = c2.modelMat[3][1];
		o2.z = c2.modelMat[3][2];
		*/
	}

	if( Material::CompareMaterial( c1.material , c2.material) )
		return true;

	return false;
}

//*************************************
// Renderer
//
//*************************************
	
Renderer* Renderer::thisObject = NULL;
	
Renderer::Renderer()
{
}

Renderer* Renderer::get()
{
	if( thisObject == NULL )
	{
		thisObject = new Renderer();
	}
	return thisObject;
}

void Renderer::beginFrame()
{
	// should lock
	drawcalls.clear();
}

void Renderer::draw( const DrawCall& drawcall )
{
	drawcalls.push_back( DrawCall(drawcall) );
}
	
void Renderer::endFrame() // unlock + sort the drawcalls + send drawcalls to the GPU
{
	//glDepthMask(true);
	//cout<<drawcalls.size()<<" drawcalls"<<endl;

	sortDrawCalls();
	int i=0;
	for( auto it = drawcalls.begin(); it != drawcalls.end(); it++ )
	{
		i++;

		const DrawCall& drawcall = (*it);

		GLint old_blendParam_src, old_blendParam_dst;
		if( drawcall.hasTransparency )
		{

#ifdef __ANDROID__
	//		glGetIntegerv( GL_BLEND_SRC_ALPHA, &old_blendParam_src);
	//		glGetIntegerv( GL_BLEND_DST_ALPHA, &old_blendParam_dst);
#else
			glGetIntegerv( GL_BLEND_SRC, &old_blendParam_src);
			glGetIntegerv( GL_BLEND_DST, &old_blendParam_dst);
#endif

			switch( drawcall.transparencyMode )
			{
				case GL_ONE_GL_ZERO:
				{
					glBlendFunc(GL_ONE,GL_ZERO);
				}
				break;
				case GL_SRC_ALPHA_GL_ONE_MINUS_SRC_ALPHA:
				{
					glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
				}
				break;
				case GL_SRC_ALPHA_GL_ONE:
				{
					glBlendFunc(GL_SRC_ALPHA,GL_ONE);
				}
				break;
				case GL_ONE_GL_ONE:
				{
					glBlendFunc(GL_ONE,GL_ONE);
				}
				break;
				case GL_SRC_ALPHA_GL_ONE_MINUS_DST_ALPHA:
				{
					glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
				}
				break;
				default:
				break;
			}
		}

		if( drawcall.disableDepthWrite )
		{
			glDepthMask(false);
		}

		GLenum errCode;
		const GLubyte *errString;
		while ((errCode = glGetError()) != GL_NO_ERROR) {
		  LOGT("DRAW","%d Error Renderer::endFrame i=%d",errCode,i);
		 // assert(false);
		}

		ShaderParams::get()->objectMatrix     = drawcall.modelMat;
		ShaderParams::get()->viewMatrix       = drawcall.viewMat;
		ShaderParams::get()->projectionMatrix = drawcall.projMat;

		glm::mat3 upperTopMV = glm::mat3( drawcall.viewMat * drawcall.modelMat );
		glm::mat3 normalMatrix = glm::transpose( glm::inverse(upperTopMV) );
		ShaderParams::get()->normalMatrix = normalMatrix;

		Shader* shader = drawcall.material->getShader(drawcall.Pass);
		assert( shader != NULL );

		shader->enable( *ShaderParams::get() );
		drawcall.material->SetupUniforms(drawcall.Pass);
		shader->Draw( drawcall.mesh );
		shader->disable();


		while ((errCode = glGetError()) != GL_NO_ERROR) {
		  LOGT("DRAW","%d Error Renderer::endFrame shader i=%d",errCode,i);
		  LOGT("DRAW"," mat %s", drawcall.material->getMaterialName().c_str());

		}

		if( drawcall.hasTransparency )
		{
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			//glBlendFunc(old_blendParam_src, old_blendParam_dst);
		}

		if( drawcall.disableDepthWrite )
		{
			glDepthMask(true);
		}

	}

}

void Renderer::sortDrawCalls()
{
	if( drawcalls.size() > 1 )
	{
		sort( drawcalls.begin(), drawcalls.end(), DrawCallComparison);
	}
}


