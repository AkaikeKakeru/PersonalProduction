﻿#include "Player.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include <cassert>
#include <Input.h>
#include <Quaternion.h>

#include "Cursor.h"
#include "DrawBasis.h"
#include <SafeDelete.h>

Input* input_ = Input::GetInstance();
CollisionManager* Player::collisionManager_ = CollisionManager::GetInstance();
DrawBasis* drawBas_ = DrawBasis::GetInstance();

Player* Player::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	Player* instance = new Player();
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

bool Player::Initialize() {
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

	worldTransform3dReticle_.Initialize();

	drawBas_->LoadTexture(1, "texture.png");

	spriteReticle_ = new Sprite();
	spriteReticle_->Initialize(drawBas_, 1);

	spriteReticle_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteReticle_->SetSize({ 64,64 });

	return true;
}

void Player::Update() {
	camera_->Update();

	Vector2 mousePosition_ =
		input_->GetMousePosition();

	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の座標を取得
	Vector3 rot = Object3d::GetRotation();

	Vector3 angleX = { 1.0f,0.0f,0.0f };
	Vector3 angleY = { 0.0f,1.0f,0.0f };
	Vector3 angleZ = { 0.0f,0.0f,1.0f };

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,0.0f };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };


	// 座標の変更を反映
	Object3d::SetRotation(rot);

	// 座標の変更を反映
	Object3d::SetPosition(position);
	
	Object3d::Update();

	Reticle();

	spriteReticle_->SetPosition(
		{ worldTransform3dReticle_.position_.x ,
		worldTransform3dReticle_.position_.y });

	spriteReticle_->SetPosition(input_->GetMousePosition());

	spriteReticle_->Update();
}

void Player::Draw() {
	Object3d::Draw(worldTransform_);
}

void Player::DrawUI() {
	spriteReticle_->Draw();
}

void Player::Finalize() {
	SafeDelete(spriteReticle_);
}

void Player::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;
}

void Player::Reticle() {
	Corsor cursor;

	worldTransform3dReticle_.position_ =
		cursor.Get3DRethiclePosition(camera_);
}
