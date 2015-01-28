#pragma once

#include <glm/glm.hpp>


class Camera
{
public:

	glm::vec3 position;
	glm::vec3 at;
	glm::vec3 original_at; // before rotation
	glm::vec3 up;

	float speed;
	float camera_rotation;
	float zoomLevel;

public:
	Camera();
	Camera( const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& haut );
	void update(float dt );

public:
	void top();
	void down();
	void left();
	void right();
	void rotate(bool clockwise);
	void zoom(bool zoomIn);
};
