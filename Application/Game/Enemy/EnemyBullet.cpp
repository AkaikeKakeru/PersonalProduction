#include "EnemyBullet.h"
#include <Input.h>
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"
#include <cassert>
#include "MyMath.h"

CollisionManager* EnemyBullet::collisionManager_ = CollisionManager::GetInstance();

EnemyBullet* EnemyBullet::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	EnemyBullet* instance = new EnemyBullet();
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

bool EnemyBullet::Initialize() {
	if (!Object3d::Initialize()) {
		return false;
	}

	//コライダ－追加

	//半径分だけ足元から浮いた座標を球の中心にする
	SetCollider(new SphereCollider(
		Vector3{ 0.0f,radius_,0.0f },
		radius_)
	);

	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

	heightAxe_ = kDefaultHeightAxe_;

	return true;
}

void EnemyBullet::Update() {
	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

	if (bulletType_ == Gun_BulletType) {
		//毎フレーム、ベロシティ分前進
		position += velocity_;
	}
	else {
		heightAxe_ -= 0.167f;

		//斧のベクトル調整
		Vector3 velocityAxe = {
			velocity_.x,
			velocity_.y + (Sin(30)+ heightAxe_),
			velocity_.z
		};
		position += velocityAxe;
	}

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

void EnemyBullet::Draw() {
	Object3d::Draw(worldTransform_);
}

void EnemyBullet::Finalize() {
}

void EnemyBullet::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;
	isDead_ = true;
}
