#include "Player.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include <cassert>
#include <Input.h>
#include <Quaternion.h>

#include "Cursor.h"
#include "DrawBasis.h"
#include <SafeDelete.h>
#include <imgui.h>

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
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

	Vector3 angleX = { 1.0f,0.0f,0.0f };
	Vector3 angleY = { 0.0f,1.0f,0.0f };
	Vector3 angleZ = { 0.0f,0.0f,1.0f };

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,0.0f };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };

	//自壊フラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});

	// 座標の回転を反映
	Object3d::SetRotation(rot);

	// 座標の変更を反映
	Object3d::SetPosition(position);

	Reticle();
	Attack();

	//弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	Object3d::Update();


	spriteReticle_->SetPosition(
		{ worldTransform3dReticle_.position_.x ,
		worldTransform3dReticle_.position_.y });

	spriteReticle_->SetPosition(input_->GetMousePosition());

	spriteReticle_->Update();
}

void Player::Draw() {
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw();
	}

	Object3d::Draw(worldTransform_);
}

void Player::DrawUI() {
	spriteReticle_->Draw();
}

void Player::DrawImgui() {
	static const int Vector3Count = 3;

	float playerPos[Vector3Count] = {
		GetPosition().x,
		GetPosition().y,
		GetPosition().z
	};

	float playerDir[Vector3Count] = {
		GetRotation().x,
		GetRotation().y,
		GetRotation().z
	};

	ImGui::Begin("Player");
	ImGui::SetWindowPos(ImVec2(700, 0));
	ImGui::SetWindowSize(ImVec2(500, 100));
	ImGui::InputFloat3("PlayerPos", playerPos);
	ImGui::InputFloat3("PlayerDir", playerDir);
	ImGui::End();
}

void Player::Finalize() {
	SafeDelete(spriteReticle_);
}

void Player::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;
}

void Player::Reticle() {
	Corsor cursor;
	//マウスカーソルから、3D照準座標を取得する
	worldTransform3dReticle_.position_ =
		cursor.Get3DRethiclePosition(camera_);

	worldTransform3dReticle_.UpdateMatrix();
}

void Player::Attack() {
	if (input_->TriggerMouse(0)) {
		//弾スピード
		const float kBulletSpeed = 2.0f;
		//毎フレーム弾が前進する速度
		Vector3 bulletVelocity = { 0.0f,0.0f,kBulletSpeed };

		//速度ベクトルを自機の向きに合わせて回転させる
		//bulletVelocity = Vector3CrossMatrix4(bulletVelocity, worldTransform_.matWorld_);
		bulletVelocity =
			Vector3{
				worldTransform3dReticle_.matWorld_.m[3][0],
				worldTransform3dReticle_.matWorld_.m[3][1],
				worldTransform3dReticle_.matWorld_.m[3][2]
		} - Vector3{
				worldTransform_.matWorld_.m[3][0],
				worldTransform_.matWorld_.m[3][1],
				worldTransform_.matWorld_.m[3][2]
		};

		bulletVelocity = Vector3Normalize(bulletVelocity) * kBulletSpeed;

		//弾の生成、初期化
		std::unique_ptr<PlayerBullet> newBullet =
			std::make_unique<PlayerBullet>();

		newBullet->Initialize();

		newBullet->SetModel(model_);

		newBullet->SetScale(worldTransform_.scale_);
		newBullet->SetRotation(worldTransform_.rotation_);
		newBullet->SetPosition(Vector3{
			worldTransform_.matWorld_.m[3][0],
			worldTransform_.matWorld_.m[3][1],
			worldTransform_.matWorld_.m[3][2]
			});

		newBullet->SetVelocity(bulletVelocity);
		newBullet->SetCamera(camera_);

		newBullet->Update();

		bullets_.push_back(std::move(newBullet));
	}
}
