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

Input* Enemy::input_ = Input::GetInstance();
CollisionManager* Enemy::collisionManager_ = CollisionManager::GetInstance();
SpriteBasis* Enemy::spriteBas_ = SpriteBasis::GetInstance();

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
	Character::SetGroupName(groupName_);
	Character::SetDefaultLife(kDefaultLife_);

	Character::SetSpeed(0.2f);

	Character::Initialize();

	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

	worldTransform3dReticle_.Initialize();

#pragma region HPスプライト
	Character::GetHPGauge()->SetRestMax(kDefaultLife_);
	Character::GetHPGauge()->SetRest(kDefaultLife_);
	Character::GetHPGauge()->SetMaxTime(kMaxTimeHP_);

	Character::GetHPGauge()->SetPosition({ 64,64 });
	Character::GetHPGauge()->SetSize({ 1,0.25f });
#pragma endregion

	return true;
}

void Enemy::Update() {

	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,0.0f };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };

	moveVector = { 0.0f,0.0f,Character::GetSpeed()};
	moveVector = Vector3CrossMatrix4(moveVector, worldTransform_.matWorld_);

	Character::SetMovePos(position);
	Character::SetMoveRota(rot);

	//発射タイマーを減らしていき、0で発射処理
	fireTimer_--;
	if (fireTimer_ <= 0) {
		//発射
		Fire();
		//発射タイマーを初期化
		fireTimer_ = kFireInterval;
	}

	Object3d::Update();

	//ライフ0で落下フラグ
	if (Character::GetLife() <= 0.0f) {
		isFall_ = true;
	}

	//落下フラグで、撤退させる
	if (isFall_) {
		Fall();
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
		posHpGauge3d.x - 64.0f+ 16.0f,
		posHpGauge3d.y - 32.0f
		});

	Character::GetHPGauge()->DecisionFluctuation();
	Character::GetHPGauge()->SetIsFluct(true);

	Character::Update();
}

void Enemy::Draw() {
	Character::Draw();
}

void Enemy::DrawUI() {
	Character::DrawUI();
}

void Enemy::DrawImgui() {
	//Character::DrawImgui();
}

void Enemy::Finalize() {
	Character::Finalize();
}

void Enemy::OnCollision(const CollisionInfo& info) {
	Character::OnCollision(info);
}

void Enemy::Fire() {
	assert(player_);

	//弾スピード
	const float kBulletSpeed = 6.0f;

	Vector3 worldPos = {
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2]
	};

	Vector3 worldPosPlayer = {
		player_->GetMatWorld().m[3][0],
		player_->GetMatWorld().m[3][1],
		player_->GetMatWorld().m[3][2],
	};

	//毎フレーム弾が前進する速度
	Vector3 bulletVelocity = worldPosPlayer - worldPos ;

	bulletVelocity = Vector3Normalize(bulletVelocity);

	bulletVelocity *= kBulletSpeed;

	//弾の生成、初期化
	std::unique_ptr<EnemyBullet> newBullet =
		std::make_unique<EnemyBullet>();

	newBullet->Initialize();

	newBullet->SetModel(Character::GetBulletModel());

	newBullet->SetScale(worldTransform_.scale_);
	newBullet->SetRotation(worldTransform_.rotation_);
	newBullet->SetPosition(Vector3{
		worldTransform_.matWorld_.m[3][0],
		worldTransform_.matWorld_.m[3][1],
		worldTransform_.matWorld_.m[3][2]
		});

	newBullet->SetVelocity(bulletVelocity);
	newBullet->SetCamera(camera_);

	newBullet->SetBulletType(bulletType_);

	newBullet->SetGameScene(Character::GetGamePlayScene() );

	newBullet->Update();

	Character::GetGamePlayScene()->AddEnemyBullet(std::move(newBullet));
}

void Enemy::Fall() {
	//Vector3 rota = GetRotation();

	//Vector3 endFallRota = {
	//	rota.x - ConvertToRadian(90.0f),
	//	rota.y,
	//	rota.z
	//};

	//Quaternion fallQua = DirectionToDirection(rota,endFallRota);

	//SetRotation( RotateVector(rota, fallQua) );

	if(Character::IsOver()) {
		Character::GetGamePlayScene()->SetIsGushing(true);
		Character::SetIsDead(true);
	}
}

void Enemy::ReSetEasePos() {
	Character::GetStartPositionEase().Reset(
		Ease::In_,
		Character::GetTimerMax(),
		{
			Object3d::GetPosition().x,
			Object3d::GetPosition().y ,
			Object3d::GetPosition() .z - 10.0f
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
			Object3d::GetRotation().x  - ConvertToRadian(90.0f),
			Object3d::GetRotation().y,
			Object3d::GetRotation().z
		}
	);
}
