#include "DebugCamera.h"
#include <MyMath.h>

Input* DebugCamera::input_ = Input::GetInstance();

void DebugCamera::Initialize(const Vector3 pos,
	const Vector3 rotaRadianAngle) {
	worldTransform_.position_ = pos;
	worldTransform_.rotation_ = rotaRadianAngle;

	viewProjection_.Initialize();
}

void DebugCamera::Update() {
	float speed = 0.0f;
	float rotaAngle = ConvertToRadian(0.0f);

	Vector3 movePos = { 0.0f,0.0f,0.0f };
	Vector3 moveRot = { 0.0f,0.0f,0.0f };

	Vector3 updatePos = worldTransform_.position_;
	Vector3 updateRota = worldTransform_.rotation_;

	// オブジェクト移動
	if (input_->PressKey(DIK_W) ||
		input_->PressKey(DIK_S) ||
		input_->PressKey(DIK_D) ||
		input_->PressKey(DIK_A)
		) {
		//前後移動
		if (input_->PressKey(DIK_W) ||
			input_->PressKey(DIK_S)) {
			if (input_->PressKey(DIK_W)) {
				speed = 0.2f;
				rotaAngle = ConvertToRadian(1.0f);
			}
			else {
				speed = -0.2f;
				rotaAngle = -ConvertToRadian(1.0f);
			}

			moveRot += Vector3{ rotaAngle,0,0 };
			movePos += Vector3{ 0,speed,0 };
		}
		//左右移動
		if (input_->PressKey(DIK_D) ||
			input_->PressKey(DIK_A)) {
			if (input_->PressKey(DIK_D)) {
				speed = 0.2f;
				rotaAngle = ConvertToRadian(1.0f);
			}
			else {
				speed -= 0.2f;
				rotaAngle -= ConvertToRadian(1.0f);
			}

			moveRot += Vector3{ 0,rotaAngle,0 };
			movePos += Vector3{ speed,0,0 };
		}
	}

	//オブジェクト回転
	if (input_->PressKey(DIK_Z) ||
		input_->PressKey(DIK_X) ||
		input_->PressKey(DIK_C) ||
		input_->PressKey(DIK_V) ||
		input_->PressKey(DIK_B) ||
		input_->PressKey(DIK_N)
		) {
		//X軸周り
		if (input_->PressKey(DIK_Z) ||
			input_->PressKey(DIK_X)) {
			if (input_->PressKey(DIK_Z)) {
				rotaAngle = ConvertToRadian(1.0f);
			}
			else {
				rotaAngle = -ConvertToRadian(1.0f);
			}

			moveRot += Vector3{ rotaAngle,0,0 };
		}

		//Y軸周り
		if (input_->PressKey(DIK_C) ||
			input_->PressKey(DIK_V)) {
			if (input_->PressKey(DIK_C)) {
				rotaAngle = ConvertToRadian(1.0f);
			}
			else {
				rotaAngle = -ConvertToRadian(1.0f);
			}

			moveRot += Vector3{ 0,rotaAngle,0 };
		}

		//Z軸周り
		if (input_->PressKey(DIK_B) ||
			input_->PressKey(DIK_N)) {
			if (input_->PressKey(DIK_B)) {
				rotaAngle = ConvertToRadian(1.0f);
			}
			else {
				rotaAngle = -ConvertToRadian(1.0f);
			}

			moveRot += Vector3{ 0,0,rotaAngle };
		}
	}

	worldTransform_.rotation_ = updateRota;
	worldTransform_.position_ = updatePos;

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
