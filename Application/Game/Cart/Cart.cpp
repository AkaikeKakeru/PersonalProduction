/*カート*/

#include "Cart.h"

#include <cassert>
#include <Quaternion.h>

#include <SafeDelete.h>

#ifdef _DEBUG
#include <imgui.h>
#endif

Cart* Cart::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	Cart* instance = new Cart();
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

bool Cart::Initialize() {
	if (!Object3d::Initialize()) {
		return false;
	}

	return true;
}

void Cart::Update() {
	camera_->Update();

	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

	// 座標の回転を反映
	Object3d::SetRotation(rot);

	// 座標の変更を反映
	Object3d::SetPosition(position);

	Object3d::Update();
}

void Cart::Draw() {
	Object3d::Draw(worldTransform_);
}

void Cart::DrawUI() {
}

void Cart::DrawImgui() {
}

void Cart::Finalize() {
}

void Cart::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;
}
