#include "Camera.h"

void Camera::updateState()
{
	direction.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
	direction.y = sin(glm::radians(rotation.x));
	direction.z = cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(this->direction, up));
	this->c_up = glm::cross(cameraRight, this->direction);
}

Camera::Camera(glm::vec3 position, glm::vec3 rotation, float fov, int scr_w, int scr_h): fov(fov), scr_w(scr_w), scr_h(scr_h)
{
	this->position = position;
	this->rotation = rotation;
	this->updateState();
}

glm::mat4 Camera::getView()
{
	glm::mat4 view;
	view = glm::lookAt(this->position,
		this->position + this->direction,
		c_up);
	return glm::mat4();
}

glm::mat4 Camera::getProj()
{
	return glm::perspective(glm::radians(fov), float(scr_w) / float(scr_h), 0.1f, 100.0f);
}

void Camera::moveFront(float x)
{
	this->position += x * this->direction;
	this->updateState();
}

void Camera::moveRight(float y)
{
	this->position += y * glm::normalize(glm::cross(this->direction, c_up));
	this->updateState();
}

void Camera::rotatePitch(float p)
{
	this->rotation.x += p;
	this->updateState();
}

void Camera::rotateYaw(float y)
{
	this->rotation.y += y;
	this->updateState();
}
