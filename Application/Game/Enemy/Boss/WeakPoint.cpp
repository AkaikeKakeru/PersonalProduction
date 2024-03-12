#include "WeakPoint.h"

#include "CollisionManager.h"
#include "SphereCollider.h"
#include "CollisionAttribute.h"
#include <cassert>

WeakPoint* WeakPoint::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	WeakPoint* instance = new WeakPoint();
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

bool WeakPoint::Initialize() {
	if (!Object3d::Initialize()) {
		return false;
	}
	//コライダ－追加

	//半径分だけ足元から浮いた座標を球の中心にする
	SetCollider(new SphereCollider(
		Vector3{ 0.0f,radiusCollider_,0.0f },
		radiusCollider_)
	);

	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

	return true;
}

void WeakPoint::Update() {
	Object3d::Update();
}

void WeakPoint::Draw() {
	Object3d::Draw();
}

void WeakPoint::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;

	if (!isBreak_) {
		SetIsBreak(true);
	}
}
