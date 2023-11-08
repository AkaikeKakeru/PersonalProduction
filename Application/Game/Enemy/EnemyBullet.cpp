#include "EnemyBullet.h"
#include <Input.h>
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"
#include <cassert>
#include "MyMath.h"
#include "GamePlayScene.h"

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

	return true;
}

void EnemyBullet::Update() {
	//移動用の一時保存ベクトル
	Vector3 move = velocity_;

	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

	//もし斧なら、軌道設定
	if (bulletType_ == Axe_BulletType) {
		//タイマーが0になるまで減少
		if (--heightTimer_ <= 0) {
			//斧の高さを落とす
			heightAxe_ -= kFallAxe_;

			//タイマー復活
			heightTimer_ = kHeightTime_;
		}

		float deceleration = 6.0f;

		//斧のベクトル調整
		move = {
			velocity_.x / deceleration,
			velocity_.y / deceleration +
				kDefaultHeightAxe_ + heightAxe_,
			velocity_.z / deceleration
		};
	}

	//毎フレーム、ベロシティ分前進
	position += move;

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

	//ダメージ量の設定
	if (bulletType_ == Gun_BulletType) {
		damage_ = kGunDamage_;
	}
	else {
		damage_ = kAxeDamage_;
	}

	gameScene_->SetNowDamageEnemy(damage_);
	gameScene_->SetNowBulletTypeEnemy(bulletType_);

	isDead_ = true;
}
