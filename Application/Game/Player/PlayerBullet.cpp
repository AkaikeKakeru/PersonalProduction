/*プレイヤー弾*/

#include "PlayerBullet.h"
#include <Input.h>
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"
#include "GamePlayScene.h"
#include <cassert>

PlayerBullet* PlayerBullet::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	PlayerBullet* instance = new PlayerBullet();

	Bullet::Create(model);

	return instance;
}

bool PlayerBullet::Initialize() {
	Bullet::Initialize();

	collider_->SetAttribute(COLLISION_ATTR_PLAYER);

	return true;
}

void PlayerBullet::Update() {
	Bullet::Update();
}

void PlayerBullet::Draw() {
	Bullet::Draw();
}

void PlayerBullet::Finalize() {
	Bullet::Finalize();
}

void PlayerBullet::OnCollision(const CollisionInfo& info) {
	Bullet::OnCollision(info);
}
