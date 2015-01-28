#include "AABB.h"

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/glew.h>
#endif
 
#include "Model.h"
#include "algorithm" 
using namespace glm;

AABB::AABB( mat4& objectMat, const Model* model )
{
	// find the min and max points :
	vec3 mini = vec3(1000,1000,1000);
	vec3 maxi = vec3(-1000,-1000,-1000);

	for( unsigned int m=0; m< model->meshes.size(); m++ )
	{
		const Model::Mesh& mesh = model->meshes[m];
		for( int v = 0; v< mesh.nbVertices; v++ )
		{
			vec3 pos = vec3( objectMat * vec4(mesh.vertex_positions[v],1.0) );

			mini.x = std::min( mini.x, pos.x );
			mini.y = std::min( mini.y, pos.y );
			mini.z = std::min( mini.z, pos.z );

			maxi.x = std::max( maxi.x, pos.x );
			maxi.y = std::max( maxi.y, pos.y );
			maxi.z = std::max( maxi.z, pos.z );
		}
	}


	minPoint = mini;
	maxPoint = maxi;
}


void AABB::Draw(mat4& objectMat)
{
#ifdef __ANDROID__
	(void)objectMat;
#else
	glPushMatrix();

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glBegin(GL_QUADS);

	// axis aligned
	glVertex3f( minPoint.x, minPoint.y, minPoint.z );
	glVertex3f( maxPoint.x, minPoint.y, minPoint.z );
	glVertex3f( maxPoint.x, maxPoint.y, minPoint.z );
	glVertex3f( minPoint.x, maxPoint.y, minPoint.z );

	glVertex3f( minPoint.x, minPoint.y, maxPoint.z );
	glVertex3f( maxPoint.x, minPoint.y, maxPoint.z );
	glVertex3f( maxPoint.x, maxPoint.y, maxPoint.z );
	glVertex3f( minPoint.x, maxPoint.y, maxPoint.z );

	glVertex3f( minPoint.x, minPoint.y, minPoint.z );
	glVertex3f( minPoint.x, minPoint.y, maxPoint.z );
	glVertex3f( minPoint.x, maxPoint.y, maxPoint.z );
	glVertex3f( minPoint.x, maxPoint.y, minPoint.z );

	glVertex3f( maxPoint.x, minPoint.y, minPoint.z );
	glVertex3f( maxPoint.x, minPoint.y, maxPoint.z );
	glVertex3f( maxPoint.x, maxPoint.y, maxPoint.z );
	glVertex3f( maxPoint.x, maxPoint.y, minPoint.z );

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glPopMatrix();
#endif

}


//http://www.3dkingdoms.com/weekly/weekly.php?a=3
int inline GetIntersection( float fDst1, float fDst2, vec3 P1, vec3 P2, vec3 &Hit)
{
if ( (fDst1 * fDst2) >= 0.0f) return 0;
if ( fDst1 == fDst2) return 0;
Hit = P1 + (P2-P1) * ( -fDst1/(fDst2-fDst1) );
return 1;
}

int inline InBox( vec3 Hit, vec3 B1, vec3 B2, const int Axis) {
if ( Axis==1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return 1;
if ( Axis==2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return 1;
if ( Axis==3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return 1;
return 0;
}

// returns true if line (L1, L2) intersects with the box (B1, B2)
// returns intersection point in Hit
int CheckLineBox( vec3 B1, vec3 B2, vec3 L1, vec3 L2, vec3 &Hit)
{
if (L2.x < B1.x && L1.x < B1.x) return false;
if (L2.x > B2.x && L1.x > B2.x) return false;
if (L2.y < B1.y && L1.y < B1.y) return false;
if (L2.y > B2.y && L1.y > B2.y) return false;
if (L2.z < B1.z && L1.z < B1.z) return false;
if (L2.z > B2.z && L1.z > B2.z) return false;
if (L1.x > B1.x && L1.x < B2.x &&
    L1.y > B1.y && L1.y < B2.y &&
    L1.z > B1.z && L1.z < B2.z)
    {Hit = L1;
    return true;}
if ( (GetIntersection( L1.x-B1.x, L2.x-B1.x, L1, L2, Hit) && InBox( Hit, B1, B2, 1 ))
  || (GetIntersection( L1.y-B1.y, L2.y-B1.y, L1, L2, Hit) && InBox( Hit, B1, B2, 2 ))
  || (GetIntersection( L1.z-B1.z, L2.z-B1.z, L1, L2, Hit) && InBox( Hit, B1, B2, 3 ))
  || (GetIntersection( L1.x-B2.x, L2.x-B2.x, L1, L2, Hit) && InBox( Hit, B1, B2, 1 ))
  || (GetIntersection( L1.y-B2.y, L2.y-B2.y, L1, L2, Hit) && InBox( Hit, B1, B2, 2 ))
  || (GetIntersection( L1.z-B2.z, L2.z-B2.z, L1, L2, Hit) && InBox( Hit, B1, B2, 3 )))
	return true;

return false;
}

bool AABB::rayIntersection( const mat4& objectMat, const vec3& p0, const vec3&  p1 )
{
	vec3 boxMin = minPoint; //vec3( objectMat * vec4(minPoint,1.0) );
	vec3 boxMax = maxPoint; //vec3( objectMat * vec4(maxPoint,1.0) );

	vec3 hit;
	if( CheckLineBox( boxMin, boxMax, p0, p1, hit) )
	{
		return true;
	}
	return false;
}

