/*エネミー*/

#include "BaseEnemy.h"

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

Input* BaseEnemy::input_ = Input::GetInstance();
CollisionManager* BaseEnemy::collisionManager_ = CollisionManager::GetInstance();
SpriteBasis* BaseEnemy::spriteBas_ = SpriteBasis::GetInstance();

BaseEnemy* BaseEnemy::Create(Model* model) {
	//オブジェクトのインスタンスを生成
	BaseEnemy* instance = new BaseEnemy();
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

bool BaseEnemy::Initialize() {
	Character::SetGroupName(groupName_);
	Character::SetDefaultLife(kDefaultLife_);

	Character::SetSpeed(0.2f);

	Character::Initialize();

	Character::collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

	worldTransform3dReticle_.Initialize();

#pragma region HPスプライト
	Gauge* newHpGauge = new Gauge();
	newHpGauge->SetRestMax(kDefaultLife_);
	newHpGauge->SetRest(kDefaultLife_);
	newHpGauge->SetMaxTime(kMaxTimeHP_);
	newHpGauge->Initialize();
	newHpGauge->SetPosition({ 64,64 });
	newHpGauge->SetSize({ 1,0.5f });
	newHpGauge->SetIsInvisible(true);
	Character::SetHPGauge(newHpGauge);
#pragma endregion

	return true;
}

void BaseEnemy::Update() {

	if(player_->IsStart()) {
		Character::GetHPGauge()->SetIsInvisible(false);
	}
	else {
		Character::GetHPGauge()->SetIsInvisible(true);
	}

	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();
	// 現在のスケールを取得
	Vector3 scale = Object3d::GetScale();

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,0.0f };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };
	//スケールベクトル
	Vector3 scaleVector = {0,0,0};

	moveVector = { 0.0f,0.0f,Character::GetSpeed() };
	moveVector = Vector3CrossMatrix4(moveVector, worldTransform_.matWorld_);

	Character::SetMovePos(position);
	Character::SetMoveRota(rot);

	//発射タイマーを減らしていき、0で発射処理
	fireTimer_--;

	if(fireTimer_ <= kFireInterval / 2 ) {
		scaleVector = { scale.x,scale.y * 1.1f,scale.z };
		Character::SetScale(scale);
	}
	//else if(fireTimer_ <= kFireInterval / 4 ){
	//	Character::SetColor({ 0.7f,0.8f,0.1f,1.0f });
	//}

	if (fireTimer_ <= 0) {
		//発射
		Attack();
		//発射タイマーを初期化
		fireTimer_ = kFireInterval;

		scaleVector = { 1,1,1 };
		Character::SetScale(scale);
	}

	Object3d::Update();

	//ライフ0で落下フラグ
	if (Character::GetLife() <= 0.0f) {
		isFall_ = true;
	}

	//落下フラグで、撤退させる
	if (isFall_) {
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

	Character::Update();
}

void BaseEnemy::Draw() {
	Character::Draw();
}

void BaseEnemy::DrawUI() {
	Character::DrawUI();
}

void BaseEnemy::DrawImgui() {
}

void BaseEnemy::Finalize() {
	Character::Finalize();
}

void BaseEnemy::OnCollision(const CollisionInfo& info) {
	Character::OnCollision(info);
}

void BaseEnemy::Attack() {
	assert(player_);

	//弾スピード
	const float kBulletSpeed = 6.0f;

	Vector3 worldPos =
		worldTransform_.posWorld_;

	Vector3 worldPosPlayer =
		player_->GetPosWorld();

	//毎フレーム弾が前進する速度
	Vector3 bulletVelocity = worldPosPlayer - worldPos;

	bulletVelocity = Vector3Normalize(bulletVelocity);

	bulletVelocity *= kBulletSpeed;

	Character::SetBulletDamage(kGunDamage_);
	Character::SetBulletVelocity(bulletVelocity);

	Character::Attack();
}

void BaseEnemy::StartMove() {
	Character::StartMove();
}

void BaseEnemy::OverMove() {
	Character::OverMove();
}

void BaseEnemy::Fall() {
	if (Character::IsOver()) {
		Character::GetGamePlayScene()->SetIsGushing(true);
		Character::SetIsDead(true);
	}
}

void BaseEnemy::ReSetEasePos() {
	Character::GetStartPositionEase().Reset(
		Ease::In_,
		Character::GetTimerMax(),
		{
			Object3d::GetPosition().x,
			Object3d::GetPosition().y ,
			Object3d::GetPosition().z - 10.0f
		},
		Object3d::GetPosition()
	);

	Character::GetStartRotationEase().Reset(
		Ease::In_,
		Character::GetTimerMax(),
		Object3d::GetRotation(),
		{
			Object3d::GetRotation().x,
			Object3d::GetRotation().y,
			Object3d::GetRotation().z
		}
	);

	Character::GetEndPositionEase().Reset(
		Ease::In_,
		60,
		Object3d::GetPosition(),
		{
			Object3d::GetPosition().x,
			kDeadBorder_,
			Object3d::GetPosition().z - 20.0f
		}
	);

	Character::GetEndRotationEase().Reset(
		Ease::In_,
		60,
		Object3d::GetRotation(),
		{
			Object3d::GetRotation().x - ConvertToRadian(90.0f),
			Object3d::GetRotation().y,
			Object3d::GetRotation().z
		}
	);
}
