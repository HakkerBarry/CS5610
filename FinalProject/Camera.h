#pragma once
#include <glm/glm.hpp>
#include "glm/ext.hpp"

class Camera
{
private:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 c_up;
	glm::vec3 rotation;
	float fov;
	int scr_w;
	int scr_h;

	void updateState();
public:
	Camera(glm::vec3 position, glm::vec3 rotation, float fov, int scr_w, int scr_h);
	glm::mat4 getView();
	glm::mat4 getProj();
	void moveFront(float x);
	void moveRight(float y);
	void rotatePitch(float p);
	void rotateYaw(float y);
};

