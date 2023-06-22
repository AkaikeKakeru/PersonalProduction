#pragma once
#include "Vector2.h"
#include "Matrix4.h"

#include "Object3d.h"
#include "Camera.h"

class Corsor {
public:
	Vector3 Get3DRethiclePosition(Camera* camera, const Object3d* object,
		const float distance, const bool isOnScreen);

private:

void GetMousePosition();
void CreateMatrixInverseViewPort(/*const Camera* camera*/);
void CheckRayDirection();

private:
	static Camera* camera_;

	Matrix4 matInverseVPV_ = Matrix4Identity();
	Vector2 mousePosition_ = {};

	Vector3 posNear_ = {};
	Vector3 posFar_ = {};
	Vector3 rayDirection_ = {};
};