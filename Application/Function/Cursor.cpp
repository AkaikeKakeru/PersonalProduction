#include "Cursor.h"
#include "Input.h"
#include <WinApp.h>

Camera* Corsor::camera_ = nullptr;

Vector3 Corsor::Get3DRethiclePosition(Camera* camera){
	camera_ = camera;

	GetMousePosition();
	CreateMatrixInverseViewPort();
	CheckRayDirection();

	Vector3 position;

	position = rayDirection_;

	return position;
}

void Corsor::GetMousePosition() {
	//マウスの座標を取得
	mousePosition_ = Input::GetInstance()->GetMousePosition();
}

void Corsor::CreateMatrixInverseViewPort(){
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

void Corsor::CheckRayDirection(){
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
	rayDirection_ = posFar_ - posNear_;
	rayDirection_ = Vector3Normalize(rayDirection_);

	//カメラからの距離
	const float kDistanceTestObject = 50.0f;

	//ニア→レイ
	rayDirection_ = (rayDirection_ - posNear_);
	rayDirection_ *= kDistanceTestObject;
}
