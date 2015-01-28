#ifndef QUATERNION_H
#define QUATERNION_H


#include <iostream>
#include <glm/glm.hpp>

class Quaternion
{
public:
	// q = w + xi + yj + zk
	float _x;
	float _y;
	float _z;
	float _w;

	Quaternion();
	Quaternion(float x, float y, float z, float w);
	Quaternion(const glm::vec3& v, float s);
	Quaternion(const Quaternion& q);
	Quaternion& operator=(const Quaternion& q);

	bool operator==(const Quaternion& q);
	

	float length();

	Quaternion conjugate() const;

	Quaternion operator/(float f) const;

	Quaternion inverse() const;

	static Quaternion Rotation(float rad_angle, glm::vec3 axis)
	{
		glm::vec3 new_axis = axis * ((float)sin(rad_angle*0.5f));
		return Quaternion( new_axis, (float)cos(rad_angle*0.5f) );
	}

	glm::mat4 RotationMatrix();

	float dot(const Quaternion& q) const;

	Quaternion& normalize();
};

std::ostream& operator<<( std::ostream &flux, const Quaternion& q );

Quaternion operator*(const Quaternion& q1, const Quaternion& q2);

Quaternion operator*(float f, const Quaternion& q);

Quaternion operator+(const Quaternion& q1, const Quaternion& q2);

// spherical linear interpolation
// t = 0 -> rend q1
// t = 1 -> rend q2
Quaternion slerp(float t, const Quaternion& q1, const Quaternion& q2);

bool equal(const Quaternion& q1, const Quaternion& q2 , float f);


#endif
