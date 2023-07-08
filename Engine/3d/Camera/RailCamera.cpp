#include "RailCamera.h"
#include <imgui.h>

Input* RailCamera::input_ = Input::GetInstance();

void RailCamera::Initialize(const Vector3 pos,
	const Vector3 rotaRadianAngle) {
	worldTransform_.position_ = pos;
	worldTransform_.rotation_ = rotaRadianAngle;

	viewProjection_.Initialize();

#ifdef _DEBUG
	{
		debugPos_[0] = { worldTransform_.position_.x };
		debugPos_[1] = { worldTransform_.position_.y };
		debugPos_[2] = { worldTransform_.position_.z };

		debugDir_[0] = { worldTransform_.rotation_.x };
		debugDir_[1] = { worldTransform_.rotation_.y };
		debugDir_[2] = { worldTransform_.rotation_.z };
	}
#endif // _DEBUG
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

//#ifdef _DEBUG
//	{
//		worldTransform_.position_ = Vector3 {
//			debugPos_[0],
//			debugPos_[1],
//			debugPos_[2], };
//
//		worldTransform_.rotation_ = Vector3 {
//			debugDir_[0],
//			debugDir_[1],
//			debugDir_[2], };
//	}
//#endif // _DEBUG

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

void RailCamera::DrawImGui() {
	debugPos_[0] = { worldTransform_.position_.x };
	debugPos_[1] = { worldTransform_.position_.y };
	debugPos_[2] = { worldTransform_.position_.z };

	debugDir_[0] = { worldTransform_.rotation_.x };
	debugDir_[1] = { worldTransform_.rotation_.y };
	debugDir_[2] = { worldTransform_.rotation_.z };

	ImGui::Begin("RailCamera");
	ImGui::SetWindowPos(ImVec2(0, 300));
	ImGui::SetWindowSize(ImVec2(500, 100));
	ImGui::SliderFloat3(
		"RailCameraPos", debugPos_, -PosRange_, PosRange_);
	ImGui::SliderFloat3(
		"RailCameraDir", debugDir_, 0, DirRange_);
	ImGui::End();
}