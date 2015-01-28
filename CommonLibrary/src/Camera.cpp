#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

Camera::Camera()
{
	position = vec3(0,0,0);
	up = vec3(0,1,0);
	at = vec3(0,0,-1);
	original_at = at;
	speed = 0.1f;
	camera_rotation = 0.0f;
	zoomLevel = 1.0;
}

Camera::Camera( const vec3& pos, const vec3& dir, const vec3& haut ) : position(pos), at(dir), up(haut)
{
	original_at = at;
	speed = 0.1f;
	camera_rotation = 0.0f;
}

void Camera::top()
{
	vec3 zdir = vec3( at.x, 0, at.z );
	zdir = normalize(zdir);
	position += zdir * speed;
}

void Camera::down()
{
	vec3 zdir = vec3( at.x, 0, at.z );
	zdir = normalize(zdir);
	position -= zdir * speed;

}

void Camera::left()
{
	vec3 xdir = cross( up, at );
	xdir = normalize( vec3( xdir.x, 0.0, xdir.z ) );
	position += xdir * speed;

}

void Camera::right()
{
	vec3 xdir = cross( up, at );
	xdir = normalize( vec3( xdir.x, 0.0, xdir.z ) );
	position -= xdir * speed;
}

void Camera::rotate(bool clockwise)
{
	// rotate around the y axis :
	camera_rotation = clockwise ? 5.0f : -5.0f;
	vec4 result = glm::rotate( mat4(1.0), camera_rotation, vec3(0,1,0) ) * vec4(at,1.0);
	at = normalize( vec3(result) );
}

void Camera::zoom(bool zoomIn)
{
	speed = 5.0/zoomIn;
	zoomLevel += zoomIn ? 1.0 : -1.0;
	position += zoomIn ? at : -at;
}


