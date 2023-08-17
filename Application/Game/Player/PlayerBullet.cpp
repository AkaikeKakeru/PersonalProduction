#include "PlayerBullet.h"
#include <Input.h>
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"
#include "GamePlayScene.h"
#include <cassert>

CollisionManager* PlayerBullet::collisionManager_ = CollisionManager::GetInstance();

PlayerBullet* PlayerBullet::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	PlayerBullet* instance = new PlayerBullet();
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

bool PlayerBullet::Initialize() {
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

void PlayerBullet::Update() {
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

void PlayerBullet::Draw() {
	Object3d::Draw(worldTransform_);
}

void PlayerBullet::Finalize() {
}

void PlayerBullet::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;

	//ダメージ量の設定
	gameScene_->SetNowDamagePlayer(damage_);

	isDead_ = true;
}
