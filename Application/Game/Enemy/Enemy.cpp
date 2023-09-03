#include "Enemy.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include <cassert>
#include <Quaternion.h>

#include <SafeDelete.h>
#include <imgui.h>

#include "EnemyBullet.h"
#include "GamePlayScene.h"

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

#pragma region HPスプライト
	hpGauge_ = new Gauge();
	hpGauge_->Initialize();

	hpGauge_->SetRest(life_);
	hpGauge_->SetLength(lengthHPGauge_);
	hpGauge_->SetMaxTime(maxTimeHP_);

	hpGauge_->SetSize({ 32,32 });
#pragma endregion

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
	//{
	//	position = Vector3 {
	//		debugPos_[0],
	//		debugPos_[1],
	//		debugPos_[2], };

	//	rot = Vector3 {
	//		debugDir_[0],
	//		debugDir_[1],
	//		debugDir_[2], };
	//}
#endif // _DEBUG


	Vector3 angleX = { 1.0f,0.0f,0.0f };
	Vector3 angleY = { 0.0f,1.0f,0.0f };
	Vector3 angleZ = { 0.0f,0.0f,1.0f };

	const float kSpeed = 0.2f;

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,0.0f };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };

	moveVector = { 0.0f,0.0f,kSpeed };
	moveVector = Vector3CrossMatrix4(moveVector, worldTransform_.matWorld_);

	// 座標の回転を反映
	Object3d::SetRotation(rot);

	// 座標の変更を反映
	Object3d::SetPosition(position);

	//発射タイマーを減らしていき、0で発射処理
	fireTimer_--;
	if (fireTimer_ <= 0) {
		//発射
		Fire();
		//発射タイマーを初期化
		fireTimer_ = kFireInterval;
	}

	Object3d::Update();

	//ライフ0でデスフラグ
	if (life_ <= 0.0f) {
		isDead_ = true;
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

	hpGauge_->GetRestSprite()->
		SetColor({ 0.2f,0.7f,0.2f,5.0f });
	hpGauge_->SetPosition({
		posHpGauge3d.x - 32.0f,
		posHpGauge3d.y - 32.0f
		});

	hpGauge_->SetRest(
		static_cast<float>(life_)
	);

	hpGauge_->DecisionFluctuation();
	hpGauge_->SetIsFluct(true);

	//通常は緑、ピンチで赤
	if (life_ <= 5.0f) {
		hpGauge_->GetRestSprite()->
			SetColor({ 0.7f,0.2f,0.2f,1.0f });
	}
	else {
		hpGauge_->GetRestSprite()->
			SetColor({ 0.2f,0.7f,0.2f,1.0f });
	}

	hpGauge_->Update();
}

void Enemy::Draw() {
	Object3d::Draw(worldTransform_);
}

void Enemy::DrawUI() {
	//hpGauge_->Draw();
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
	hpGauge_->Finalize();
}

void Enemy::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;

	isDamage_ = true;
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

	newBullet->SetModel(bulletModel_);

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

	newBullet->SetGameScene(gameScene_);

	newBullet->Update();

	gameScene_->AddEnemyBullet(std::move(newBullet));
}
