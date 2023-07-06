#include "Enemy.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include <cassert>
#include <Quaternion.h>

#include <SafeDelete.h>
#include <imgui.h>

Input* Enemy::input_ = Input::GetInstance();
CollisionManager* Enemy::collisionManager_ = CollisionManager::GetInstance();
DrawBasis* Enemy::drawBas_ = DrawBasis::GetInstance();

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

	return instance;
}

bool Enemy::Initialize() {
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

	worldTransform3dReticle_.Initialize();

	SetScale({ 1.0f, 1.0f, 1.0f });
	SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(180.0f)));
	SetPosition({ 0.0f,0.0f,100.0f });


	//drawBas_->LoadTexture(1, "texture.png");

	//spriteReticle_ = new Sprite();
	//spriteReticle_->Initialize(drawBas_, 1);

	//spriteReticle_->SetAnchorPoint({ 0.5f, 0.5f });
	//spriteReticle_->SetSize({ 64,64 });

#ifdef _DEBUG
	{
		debugPos_[0] = { GetPosition().x };
		debugPos_[1] = { GetPosition().y };
		debugPos_[2] = { GetPosition().z };

		debugDir_[0] = { GetRotation().x };
		debugDir_[1] = { GetRotation().y };
		debugDir_[2] = { GetRotation().z };
	}
#endif // _DEBUG

	return true;
}

void Enemy::Update() {
	camera_->Update();

	Vector2 mousePosition_ =
		input_->GetMousePosition();

	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

#ifdef _DEBUG
	{
		position = Vector3 {
			debugPos_[0],
			debugPos_[1],
			debugPos_[2], };

		rot = Vector3 {
			debugDir_[0],
			debugDir_[1],
			debugDir_[2], };
	}
#endif // _DEBUG

	Vector3 angleX = { 1.0f,0.0f,0.0f };
	Vector3 angleY = { 0.0f,1.0f,0.0f };
	Vector3 angleZ = { 0.0f,0.0f,1.0f };

	const float kSpeed = 0.2f;

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,0.0f };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };

	////自壊フラグの立った弾を削除
	//bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
	//	return bullet->IsDead();
	//	});

	moveVector = { 0.0f,0.0f,kSpeed };
	moveVector = Vector3CrossMatrix4(moveVector, worldTransform_.matWorld_);

	position += moveVector;

	// 座標の回転を反映
	Object3d::SetRotation(rot);

	// 座標の変更を反映
	Object3d::SetPosition(position);

	//Reticle();
	//Attack();

	//弾更新
	//for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
	//	bullet->Update();
	//}

	Object3d::Update();


	//spriteReticle_->SetPosition(
	//	{ worldTransform3dReticle_.position_.x ,
	//	worldTransform3dReticle_.position_.y });

	//spriteReticle_->SetPosition(input_->GetMousePosition());

	//spriteReticle_->Update();
}

void Enemy::Draw() {
	Object3d::Draw(worldTransform_);
}

void Enemy::DrawUI() {
	//spriteReticle_->Draw();
}

void Enemy::DrawImgui() {
	debugPos_[0] = { GetPosition().x };
	debugPos_[1] = { GetPosition().y };
	debugPos_[2] = { GetPosition().z };

	debugDir_[0] = { GetRotation().x };
	debugDir_[1] = { GetRotation().y };
	debugDir_[2] = { GetRotation().z };

	ImGui::Begin("Enemy");
	ImGui::SetWindowPos(ImVec2(700, 0));
	ImGui::SetWindowSize(ImVec2(500, 100));
	ImGui::SliderFloat3(
		"EnemyPos", debugPos_, -PosRange_, PosRange_);
	ImGui::SliderFloat3(
		"EnemyDir", debugDir_, 0, DirRange_);
	ImGui::End();
}

void Enemy::Finalize() {
	//SafeDelete(spriteReticle_);
}

void Enemy::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;
}
