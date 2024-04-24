/*エネミー*/

#include "Enemy.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include "EnemyBullet.h"
#include "GamePlayScene.h"

#include <cassert>
#include <Quaternion.h>
#include <SafeDelete.h>

#ifdef _DEBUG
#include <imgui.h>
#endif

Enemy* Enemy::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	Enemy* instance = new Enemy();
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

	Character::Create();

	return instance;
}

bool Enemy::Initialize() {

	BaseEnemy::Initialize();

#pragma region カート

	Cart* newCart = Cart::Create();
	newCart->Initialize();
	newCart->SetModel(
		ObjectManager::GetInstance()->
		GetModel(
			ObjectManager::cartModel_
		)
	);
	newCart->SetCamera(camera_);
	Character::SetCart(newCart);

#pragma endregion

	return true;
}

void Enemy::Update() {
	BaseEnemy::Update();

	Character::GetCart()->SetPosition(Vector3{
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1] + kConfigCartPosY_,
		worldTransform_.matWorld_.m[3][2]
		}
	);

	Character::Update();
}

void Enemy::Draw() {
	BaseEnemy::Draw();
}

void Enemy::DrawUI() {
	BaseEnemy::DrawUI();
}

void Enemy::DrawImgui() {
	BaseEnemy::DrawImgui();
}

void Enemy::Finalize() {
	BaseEnemy::Finalize();
}

void Enemy::OnCollision(const CollisionInfo& info) {
	BaseEnemy::OnCollision(info);
}

void Enemy::Attack() {
	BaseEnemy::Attack();
}

void Enemy::StartMove() {
	BaseEnemy::StartMove();
}

void Enemy::OverMove() {
	BaseEnemy::OverMove();
}

void Enemy::Fall() {
	BaseEnemy::Fall();
}

void Enemy::ReSetEasePos() {
	BaseEnemy::ReSetEasePos();
}
