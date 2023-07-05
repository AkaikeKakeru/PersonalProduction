#include "RailCamera.h"

Input* RailCamera::input_ = Input::GetInstance();

void RailCamera::Initialize(const Vector3 pos,
	const Vector3 rotaRadianAngle) {
	worldTransform_.position_ = pos;
	worldTransform_.rotation_ = rotaRadianAngle;

	viewProjection_.Initialize();
}

void RailCamera::Update() {
	Vector3 move = { 0.0f,0.0f,0.01f };

	//worldTransform_.position_ += move;

	// オブジェクト移動
	if (input_->PressKey(DIK_W) ||
		input_->PressKey(DIK_S) ||
		input_->PressKey(DIK_D) ||
		input_->PressKey(DIK_A)) {
		if (input_->PressKey(DIK_W)) {
			worldTransform_.position_ += Vector3{ 0,0.1f,0 };
			worldTransform_.rotation_ += Vector3{ ConvertToRadian(1.0f),0,0};
		}
		else if (input_->PressKey(DIK_S)) {
			worldTransform_.position_ -= Vector3{ 0,0.1f,0 };
			worldTransform_.rotation_ -= Vector3{ ConvertToRadian(1.0f),0,0 };
		}
		if (input_->PressKey(DIK_D)) {
			worldTransform_.position_ += Vector3{ 0.1f,0,0 };
			worldTransform_.rotation_ += Vector3{ 0,ConvertToRadian(1.0f),0 };
		}
		else if (input_->PressKey(DIK_A)) {
			worldTransform_.position_ -= Vector3{ 0.1f,0,0 };
			worldTransform_.rotation_ -= Vector3{ 0,ConvertToRadian(1.0f),0 };
		}
	}

	worldTransform_.UpdateMatrix();

	viewProjection_.eye_ = {
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2] };

	//ワールド前方ベクトル
	Vector3 forward(0, 0, 1);
	//レールカメラの回転を反映
	forward = Vector3CrossMatrix4(forward, worldTransform_.matWorld_);

	//視点から前方に進んだ位置が注視点
	viewProjection_.target_ = viewProjection_.eye_ + forward;

	//ワールド上方ベクトル
	Vector3 up(0, 1, 0);
	//レールカメラの回転を反映
	viewProjection_.up_ = Vector3CrossMatrix4(up, worldTransform_.matWorld_);

	//ビュープロジェクション更新
	viewProjection_.UpdateMatrix();
	viewProjection_.TransferMatrix();
}
