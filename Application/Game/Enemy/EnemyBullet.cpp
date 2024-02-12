#include "EnemyBullet.h"
#include <Input.h>
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"
#include <cassert>
#include "MyMath.h"
#include "GamePlayScene.h"

EnemyBullet* EnemyBullet::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	EnemyBullet* instance = new EnemyBullet();

	Bullet::Create(model);

	return instance;
}

bool EnemyBullet::Initialize() {
	Bullet::Initialize();

	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

	return true;
}

void EnemyBullet::Update() {
	Vector3 vel = Bullet::GetVelocity();
	Vector3 move{};

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
			vel.x / deceleration,
			vel.y / deceleration +
				kDefaultHeightAxe_ + heightAxe_,
			vel.z / deceleration
		};
	}
	Bullet::SetVelocity(move);
	Bullet::Update();
}

void EnemyBullet::Draw() {
	Object3d::Draw(worldTransform_);
}

void EnemyBullet::Finalize() {
}

void EnemyBullet::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;

	float damage = 0.0f;

	//ダメージ量の設定
	if (bulletType_ == Gun_BulletType) {
		damage = kGunDamage_;
	}
	else {
		damage = kAxeDamage_;
	}

	Bullet::SetDamage(damage);

	Bullet::OnCollision(info);
}
