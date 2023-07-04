﻿#include "PlayerBullet.h"
#include <Input.h>
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"
#include <cassert>

//Input* input_ = Input::GetInstance();
CollisionManager* PlayerBullet::collisionManager_ = CollisionManager::GetInstance();
//DrawBasis* drawBas_ = DrawBasis::GetInstance();

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

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,0.0f };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };

	//速度
	float speed = 2.0f;

	//毎フレーム前進
	moveVector = { 0.0f,0.0f,speed };

	rot += rotVector;
	position += moveVector;

	// 座標の回転を反映
	Object3d::SetRotation(rot);
	// 座標の変更を反映
	Object3d::SetPosition(position);

	Object3d::Update();
}

void PlayerBullet::Draw() {
	Object3d::Draw(worldTransform_);
}

void PlayerBullet::Finalize() {
}

void PlayerBullet::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;
}