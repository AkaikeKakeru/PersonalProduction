#include "Enemy.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include <cassert>
#include <Input.h>
#include <Quaternion.h>

#include "DrawBasis.h"
#include <SafeDelete.h>
#include <imgui.h>

Input* input_ = Input::GetInstance();
CollisionManager* Enemy::collisionManager_ = CollisionManager::GetInstance();
DrawBasis* drawBas_ = DrawBasis::GetInstance();

Enemy* Enemy::Create(Model* model) {
	return nullptr;
}

bool Enemy::Initialize() {
	return false;
}

void Enemy::Update() {
}

void Enemy::Draw() {
}

void Enemy::DrawUI() {
}

void Enemy::DrawImgui() {
}

void Enemy::Finalize() {
}

void Enemy::OnCollision(const CollisionInfo& info) {
}
