#include "Player.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include <cassert>
#include <Input.h>
#include <Quaternion.h>

Input* input_ = Input::GetInstance();
CollisionManager* Player::collisionManager_ = CollisionManager::GetInstance();

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

	return true;
}

void Player::Update() {

	Vector2 mousePosition_ =
		input_->GetMousePosition();

	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の座標を取得
	Vector3 rot = Object3d::GetRotation();

		//移動スピード
		float moveSpeed = 0.4f;
		//回転スピード
		float rotSpeed = ConvertToRadian(90.0f);

		Vector3 angleX = { 1.0f,0.0f,0.0f };
		Vector3 angleY = { 0.0f,1.0f,0.0f };
		Vector3 angleZ = { 0.0f,0.0f,1.0f };

		//移動ベクトル
		Vector3 moveVector = { 0.0f,0.0f,0.0f };
		//回転ベクトル
		Vector3 rotVector = { 0.0f,0.0f,0.0f };

		//移動後の座標を計算
		if (input_->TriggerKey(DIK_UP)) {
			rotVector = CreateRotationVector(
				angleX, rotSpeed);
		}

		else if (input_->TriggerKey(DIK_DOWN)) {
			rotVector = CreateRotationVector(
				angleX, -rotSpeed);
		}

		if (input_->TriggerKey(DIK_RIGHT)) {
			rotVector = CreateRotationVector(
				angleY, rotSpeed);
		}

		else if (input_->TriggerKey(DIK_LEFT)) {
			rotVector = CreateRotationVector(
				angleY, -rotSpeed);
		}

		moveVector.z = -moveSpeed;

		rot += rotVector;

		moveVector = Vector3TransformCoord(moveVector, worldTransform_.matWorld_);

		position = moveVector;


	// 座標の変更を反映
	Object3d::SetRotation(rot);

	// 座標の変更を反映
	Object3d::SetPosition(position);

	camera_->Update();
	Object3d::Update();
}

void Player::Draw() {
	Object3d::Draw(worldTransform_);
}

void Player::Finalize() {
}

void Player::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;
}
