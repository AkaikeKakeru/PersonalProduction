/*弾*/

#include "Bullet.h"
#include <Input.h>
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"
#include "GamePlayScene.h"
#include <cassert>

CollisionManager* Bullet::collisionManager_ = CollisionManager::GetInstance();

Bullet* Bullet::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	Bullet* instance = new Bullet();
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

bool Bullet::Initialize() {
	if (!Object3d::Initialize()) {
		return false;
	}

	//コライダ－追加

	//半径分だけ足元から浮いた座標を球の中心にする
	SetCollider(new SphereCollider(
		Vector3{ 0.0f,radius_,0.0f },
		radius_)
	);

	collider_->SetAttribute(COLLISION_ATTR_PLAYER);

	return true;
}

void Bullet::Update() {
	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

	//毎フレーム、ベロシティ分前進
	position += velocity_;

	// 座標の回転を反映
	Object3d::SetRotation(rot);
	// 座標の変更を反映
	Object3d::SetPosition(position);

	Object3d::Update();

	//時間経過で自壊させる
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void Bullet::Draw() {
	Object3d::Draw(worldTransform_);
}

void Bullet::Finalize() {
}

void Bullet::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;

	isDead_ = true;
}

void Bullet::SetAttribute(unsigned short attr) {
	collider_->SetAttribute(attr);
}
