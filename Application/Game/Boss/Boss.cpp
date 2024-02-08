#include "Boss.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include <cassert>

Boss* Boss::Create(
	Model* model) {
	//オブジェクトのインスタンスを生成
	Boss* instance = new Boss();
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

bool Boss::Initialize() {
	Character::SetGroupName(groupName_);
	Character::SetDefaultLife(kDefaultLife_);

	Character::SetSpeed(0.2f);

	Character::Initialize();

	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

#pragma region HPスプライト
	Character::GetHPGauge()->SetRestMax(kDefaultLife_);
	Character::GetHPGauge()->SetRest(kDefaultLife_);
	Character::GetHPGauge()->SetMaxTime(kMaxTimeHP_);

	Character::GetHPGauge()->SetPosition({ 64,64 });
	Character::GetHPGauge()->SetSize({ 1,0.25f });
#pragma endregion

	return false;
}

void Boss::Update() {
	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,0.0f };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };

	moveVector = { 0.0f,0.0f,Character::GetSpeed() };
	moveVector = Vector3CrossMatrix4(moveVector, worldTransform_.matWorld_);

	Character::SetMovePos(position);
	Character::SetMoveRota(rot);

	Object3d::Update();

	//ライフ0で落下フラグ
	if (Character::GetLife() <= 0.0f) {
		isOver_ = true;
	}

	//落下フラグで、撤退させる
	if (isOver_) {
		OverMove();
	}

	//HPゲージの変動
	Vector3 posHpGauge3d = {
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2]
	};

	Matrix4 matViewPort = Matrix4Identity();
	matViewPort.m[0][0] = static_cast<float>(WinApp::Win_Width) / 2;
	matViewPort.m[1][1] = static_cast<float>(-(WinApp::Win_Height)) / 2;
	matViewPort.m[3][0] = static_cast<float>(WinApp::Win_Width) / 2;
	matViewPort.m[3][1] = static_cast<float>(WinApp::Win_Height) / 2;

	Matrix4 matVPV = camera_->GetViewMatrix()
		* camera_->GetProjectionMatrix()
		* matViewPort;

	posHpGauge3d = Vector3TransformCoord(posHpGauge3d, matVPV);

	Character::GetHPGauge()->GetRestSprite()->
		SetColor({ 0.2f,0.7f,0.2f,5.0f });
	Character::GetHPGauge()->SetPosition({
		posHpGauge3d.x - 64.0f + 16.0f,
		posHpGauge3d.y - 32.0f
		});

	Character::GetHPGauge()->DecisionFluctuation();
	Character::GetHPGauge()->SetIsFluct(true);

	Character::GetCart()->SetPosition(Vector3{
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1] + kConfigCartPosY_,
		worldTransform_.matWorld_.m[3][2]
		}
	);

	Character::Update();
}

void Boss::Draw() {
	Character::Draw();
}

void Boss::DrawUI() {
	Character::DrawUI();
}

void Boss::DrawImgui() {
	Character::DrawImgui();
}

void Boss::Finalize() {
	Character::Finalize();
}

void Boss::OnCollision(
	const CollisionInfo& info) {
	Character::OnCollision(info);
}

void Boss::StartMove() {
	Character::StartMove();
}

void Boss::OverMove() {
	Character::OverMove();
}
