#include "Camera.h"
#include <cmath>

Camera::Camera(): fov(45), res_x(1920), res_y(1080), z_near(0.01f), z_far(200.f), position(0, 0, 1)
{
    setTarget(Vec3<float>(0, 0, 0));
}

Matrix4<float> Camera::getView()
{
    Vec3<float> up(0.0f, 1.0f, 0.0f);
    Vec3<float> cameraRight = up.Cross(this->direction);
    cameraRight.Normalize();
    Vec3<float> cameraUp = cameraRight.Cross(this->direction);
    cameraUp.Normalize();

    Matrix4<float> view
    ({ cameraRight.x, cameraUp.x, direction.x, 0,
       cameraRight.y, cameraUp.y, direction.y, 0,
       cameraRight.z, cameraUp.z, direction.z, 0,
       0, 0, 0, 1 });

    Matrix4<float> cameraTrans;
    cameraTrans.SetTranslation(Vec3<float>(-this->position.x, -this->position.y, -this->position.z));
    view = cameraTrans * view;
    return view;
}

Matrix4<float> Camera::getPerspective()
{
    Matrix4<float> pers;
    pers.SetPerspective(this->fov, this->res_x / this->res_y, this->z_near, this->z_far);
    return pers;
}

void Camera::setPosition(Vec3<float> position)
{
    this->position = position;
    this->direction = this->position - this->target;
}

void Camera::setTarget(Vec3<float> target)
{
    this->target = target;
    this->direction = this->position - this->target;
}

void Camera::setCutZPlane(float near , float far)
{
    this->z_near = near;
    this->z_far = far;
}

void Camera::setResolution(float x, float y)
{
    res_x = x;
    res_y = y;
}

float Camera::get_degrees(float input)
{
    float pi = std::acos(-1);
    float halfC = pi / 180;
    return input * halfC;
}
