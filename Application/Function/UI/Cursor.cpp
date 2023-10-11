#include "Cursor.h"
#include "Input.h"
#include <WinApp.h>
#include <cassert>

Camera* Corsor::camera_ = nullptr;

Vector3 Corsor::Get3DRethiclePosition(Camera* camera) {
	camera_ = camera;

	CreateMatrixInverseViewPort();
	CheckRayDirection();

	return rayDirection_;
}

void Corsor::CreateMatrixInverseViewPort() {
	Matrix4 matViewPort = Matrix4Identity();
	matViewPort.m[0][0] = static_cast<float>(WinApp::Win_Width) / 2;
	matViewPort.m[1][1] = static_cast<float>(-(WinApp::Win_Height)) / 2;
	matViewPort.m[3][0] = static_cast<float>(WinApp::Win_Width) / 2;
	matViewPort.m[3][1] = static_cast<float>(WinApp::Win_Height) / 2;

	Matrix4 matVPV = camera_->GetViewMatrix()
		* camera_->GetProjectionMatrix()
		* matViewPort;

	//上を逆行列化
	matInverseVPV_ = Matrix4Inverse(matVPV);
}

void Corsor::CheckRayDirection() {
	assert(distance_);
	
	//マウスの座標を取得
	Vector2 mousePosition_ = 
		Input::GetInstance()->GetMousePosition();

	//ニア
	posNear_ = Vector3(
		mousePosition_.x,
		mousePosition_.y,
		0);
	posNear_ = Vector3TransformCoord(posNear_, matInverseVPV_);
	//ファー
	posFar_ = Vector3(
		mousePosition_.x,
		mousePosition_.y,
		1);
	posFar_ = Vector3TransformCoord(posFar_, matInverseVPV_);

	//レイ
	rayDirection_ = posNear_ - posFar_;
	rayDirection_ = Vector3Normalize(rayDirection_);

	//ニア→レイ
	rayDirection_ = posNear_ - (rayDirection_ * distance_);
}
