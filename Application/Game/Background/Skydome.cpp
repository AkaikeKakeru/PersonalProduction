#include "Skydome.h"

#include <cassert>
#include <Quaternion.h>

#include <SafeDelete.h>
#include <imgui.h>

Skydome* Skydome::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	Skydome* instance = new Skydome();
	if (instance == nullptr) {
		return nullptr;
	}

	//初期化
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
	}

	//モデルのセット
	if (model) {
		instance->SetModel(model);
	}

	return instance;
}

bool Skydome::Initialize() {
	if (!Object3d::Initialize()) {
		return false;
	}

	//SetScale({ 1.0f, 1.0f, 1.0f });
	//SetRotation(CreateRotationVector(
	//	{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	//SetPosition({ 0.0f,-5.0f,30.0f });

#ifdef _DEBUG
	{
		debugPos_[0] = { GetPosition().x };
		debugPos_[1] = { GetPosition().y };
		debugPos_[2] = { GetPosition().z };

		debugDir_[0] = { GetRotation().x };
		debugDir_[1] = { GetRotation().y };
		debugDir_[2] = { GetRotation().z };
	}
#endif // _DEBUG

	return true;
}

void Skydome::Update() {
	camera_->Update();

	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

#ifdef _DEBUG
	//{
	//	position = Vector3 {
	//		debugPos_[0],
	//		debugPos_[1],
	//		debugPos_[2], };

	//	rot = Vector3 {
	//		debugDir_[0],
	//		debugDir_[1],
	//		debugDir_[2], };
	//}
#endif // _DEBUG

	Vector3 angleX = { 1.0f,0.0f,0.0f };
	Vector3 angleY = { 0.0f,1.0f,0.0f };
	Vector3 angleZ = { 0.0f,0.0f,1.0f };

	//速さ
	float speed = 10.0f;

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,-speed };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };

	//position += moveVector;

	if (position.z <= -worldTransform_.scale_.z) {
		position.z = worldTransform_.scale_.z;
	}

	// 座標の回転を反映
	Object3d::SetRotation(rot);

	// 座標の変更を反映
	Object3d::SetPosition(position);

	Object3d::Update();
}

void Skydome::Draw() {
	Object3d::Draw(worldTransform_);
}

void Skydome::DrawUI() {
}

void Skydome::DrawImgui() {
	debugPos_[0] = { GetPosition().x };
	debugPos_[1] = { GetPosition().y };
	debugPos_[2] = { GetPosition().z };

	debugDir_[0] = { GetRotation().x };
	debugDir_[1] = { GetRotation().y };
	debugDir_[2] = { GetRotation().z };

	ImGui::Begin("Skydome");
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(500, 100));
	ImGui::SliderFloat3(
		"SkydomePos", debugPos_, -PosRange_, PosRange_);
	ImGui::SliderFloat3(
		"SkydomeDir", debugDir_, 0, DirRange_);
	ImGui::End();
}

void Skydome::Finalize() {
}

void Skydome::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;
}