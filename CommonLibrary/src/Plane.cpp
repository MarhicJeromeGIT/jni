#include "Plane.h"

using namespace glm;

Plane::Plane( const vec3& p0, const vec3& p1, const vec3& p2)
{
	vec3 n = cross( p1-p0, p2-p0 );
	n = normalize(n);

	D = - dot(n,p0);
	ABC = n;

	point = p0;
}

Plane::Plane( const vec3& normal, const vec3& p0 )
{
	point = p0;
	vec3 n = normalize(normal); 
	D = - dot(n,p0);
	ABC = n;
}

Plane::~Plane(void)
{
}