#pragma once
#ifndef CAMERA
#define CAMERA

#include "cyMatrix.h"
#include "cyVector.h"

using namespace cy;

class Camera
{
public:
	Camera();

	Vec3<float> position;

	// target
	Vec3<float> target;

	// camera Direction
	Vec3<float> direction;

	// camera cut plane z (in view space)
	float z_near, z_far;

	// resolution
	float res_x, res_y;

	// field of view
	float fov;

	// get View matrix in XYZ order
	Matrix4<float> getView();

	Matrix4<float> getPerspective();

	// function for reset position
	void setPosition(Vec3<float> position);

	// function for reset target
	void setTarget(Vec3<float> target);

	void setCutZPlane(float near, float far);

	void setResolution(float x, float y);

private:
	float get_degrees(float input);
};

#endif