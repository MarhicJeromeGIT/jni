#include "Bones.h"
#include <glm/gtc/matrix_transform.hpp> 

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif


using namespace glm;

//#include "mathematiques.h"
#include <string.h>

Bone* Bone::findBoneByName(const char* n)
{
	if( strcmp(name,n) == 0 )
	{
		return this;
	}
	else
	{
		for( int i=0; i<childs.size(); i++ )
		{
			Bone* found = childs[i]->findBoneByName(n);
			if( found != NULL )
				return found;
		}
	}
	return NULL;
}

Bone* Bone::findBoneByIndex(unsigned int index)
{
	if( BoneIndex == index )
	{
		return this;
	}
	else
	{
		for( int i=0; i<childs.size(); i++ )
		{
			Bone* found = childs[i]->findBoneByIndex(index);
			if( found != NULL )
				return found;
		}
	}
	return NULL;
}

Bone::Bone(  const char* n, Bone* p, const vec3& o, const vec3& r)
{
	strcpy(name,n);

	if( p != NULL )
		p->addChild(this);

	BoneIndex = 0;

    offset = o;
    rotation = r;
    position = vec4(0);
 	parent = p;

	//color = vec3(randf(),randf(),randf());
	color = vec3(0,0,1);

	localTransform = mat4(1.0);
	// rotate + translate
//	glRotatef( rot.z, 0, 0, 1 );
//	glRotatef( rot.x, 1, 0, 0 );
//	glRotatef( rot.y, 0, 1, 0 );
//	localTransform *= glm::translate(  glm::mat4(1.0f), offset );
//	localTransform *= glm::rotate(localTransform, rotation.x, vec3(1,0,0) );
//	localTransform *= glm::rotate(localTransform, rotation.z, vec3(0,0,1) );
//	localTransform *= glm::rotate(localTransform, rotation.y, vec3(0,1,0) );

	localTransform = glm::translate(  glm::mat4(1.0f), offset )  * glm::rotate(localTransform, rotation.z, vec3(0,0,1) )*  glm::rotate(mat4(1.0), rotation.x, vec3(1,0,0) )  *glm::rotate(localTransform, rotation.y, vec3(0,1,0) );

	computeWorldTransform();
	computeBinv();
}

Bone::Bone( Bone* p, const mat4& transform)
{
	if( p != NULL )
		p->addChild(this);

    position = vec4(0);
 	parent = p;

	//color = vec3(randf(),randf(),randf());
	color = vec3(0,0,1);

	localTransform = transform;
	computeWorldTransform();
	computeBinv();
}

void Bone::computeWorldTransform()
{
    if( parent != NULL )
    {
        worldTransform = parent->worldTransform * localTransform;
    }
    else
    {
        worldTransform = localTransform;
    }
}

void Bone::computeBinv()
{
	Binv = inverse(worldTransform);
	Binv = transpose(Binv);

	// Needed to stay consistent with Assimp (don't know why it's different)
	// Binv[ligne][colonne]
	float temp = Binv[0][1];
	Binv[0][1] = -Binv[0][2];
	Binv[0][2] = temp;

	temp = Binv[1][1];
	Binv[1][1] = -Binv[1][2];
	Binv[1][2] = temp;

	temp = Binv[2][1];
	Binv[2][1] = -Binv[2][2];
	Binv[2][2] = temp;

	//Binv = transpose(Binv);

	for(int i=0; i<childs.size(); i++ )
    {
        childs[i]->computeBinv();
    }
}

void Bone::draw()
{	
#ifndef __ANDROID__

//	computeWorldTransform();

	glColor3f(0,1,0);
	mat4 current;
	glGetFloatv(GL_MODELVIEW_MATRIX, &current[0][0] );
	glPushMatrix();
	glMultMatrixf( &localTransform[0][0] );

	glPointSize(5.0);

	glBegin(GL_POINTS);
	glVertex3f(0,0,0);
	glEnd();

	// draw childs :
	for(int i=0; i<childs.size(); i++ )
	{
		childs[i]->draw();
	}

	glPopMatrix();

	glLineWidth(2.0);
	glColor3f(0,0,1);
	current = transpose(current);
	vec4 pos1 = current * vec4(0,0,0,1);

	current  =  localTransform * current;
	vec4 pos2 = current * vec4(0,0,0,1);

	glBegin(GL_LINES);
	glVertex3f(pos1.x,pos1.y,pos1.z);
	glVertex3f(pos2.x,pos2.y,pos2.z);
	glEnd();

/*
	glPushMatrix();
		
	glColor3f(color.x,color.y,color.z);
	glMultMatrixf(&localTransform[0][0] );
	glBegin(GL_POINTS);
	glVertex3f(0,0,0);
	glEnd();

    // draw childs :
    for(int i=0; i<childs.size(); i++ )
    {
        childs[i]->draw();
    }

	glPopMatrix();
*/
	// fonctionne.
	/*
	glPushMatrix();
	glLoadIdentity();	
		gluLookAt(0.0f,5.0f,100.0f,
			  0,5,0,
			  0.f,1.f,0.f);

	glColor3f(1,0,0);
	glMultMatrixf(&worldTransform[0][0] );
	glBegin(GL_POINTS);
	glVertex3f(0,0,0);
	glEnd();
	glPopMatrix();
	*/

#endif // ! __ANDROID__
}

void Bone::addChild( Bone* child )
{ 
      childs.push_back(child);
}

void Bone::setBonePosition( vec3 pos )
{
	vec4 pos4(pos,1.0);
	for( int i=0; i< 4; i++ )
	{
		localTransform[3][i] = pos4[i];
	}

}

void Bone::recomputeWorldTransform()
{
	computeWorldTransform();
	 // draw childs :
    for(int i=0; i<childs.size(); i++ )
    {
        childs[i]->recomputeWorldTransform();
    }
}
