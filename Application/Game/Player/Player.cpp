#include "Player.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include <cassert>

#include <Quaternion.h>

#include "Cursor.h"
#include <SafeDelete.h>
#include <imgui.h>

#include "GamePlayScene.h"
#include "PlayerBullet.h"
#include <Framework.h>

Input* Player::input_ = Input::GetInstance();
CollisionManager* Player::collisionManager_ = CollisionManager::GetInstance();
SpriteBasis* Player::spriteBas_ = SpriteBasis::GetInstance();

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

	SetScale({ 1.0f, 1.0f, 1.0f });
	SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	SetPosition({ 0.0f,-5.0f,30.0f });

	spriteReticle_ = new Sprite();
	spriteReticle_->Initialize(1);

	spriteReticle_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteReticle_->SetSize({ 64,64 });

#pragma region HPスプライト
	hpGauge_ = new Gauge();
	hpGauge_->Initialize();

	hpGauge_->SetRestMax(life_);
	hpGauge_->SetRest(life_);
	hpGauge_->SetMaxTime(maxTimeHP_);

	hpGauge_->SetPosition({ 64,64 });
	hpGauge_->SetSize({ 1,1 });
#pragma endregion

#pragma region 残弾数スプライト
	bulletGauge_ = new Gauge();
	bulletGauge_->Initialize();

	bulletGauge_->GetRestSprite()->SetColor({ 0.2f,0.7f,0.2f,0.5f });
	bulletGauge_->SetRestMax(static_cast<float>(remainBulletCount_));
	bulletGauge_->SetRest(static_cast<float>(remainBulletCount_));
	bulletGauge_->SetMaxTime(maxTimeBullet_);

	bulletGauge_->SetPosition({ 64,64 });
	bulletGauge_->SetSize({ 0.5f,0.5f });
#pragma endregion

	//テキスト
	text_ = new Text();
	text_->Initialize(Framework::kTextTextureIndex_);

	shake_.SetWidthSwing(5.0f);

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

void Player::Update() {
	camera_->Update();

	Vector2 mousePosition_ =
		input_->GetMousePosition();

	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

#ifdef _DEBUG
	{
		position = Vector3{
			debugPos_[0],
			debugPos_[1],
			debugPos_[2], };

		rot = Vector3{
			debugDir_[0],
			debugDir_[1],
			debugDir_[2], };
	}
#endif // _DEBUG

	Vector3 angleX = { 1.0f,0.0f,0.0f };
	Vector3 angleY = { 0.0f,1.0f,0.0f };
	Vector3 angleZ = { 0.0f,0.0f,1.0f };

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,0.0f };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };

	//シェイク後座標
	Vector3 shakePos =  Object3d::GetPosition();

	//入力で隠れフラグ操作
	if (input_->PressMouse(1)) {
		isHide_ = true;
	}
	else {
		isHide_ = false;
	}

	Reticle();

	//隠れフラグが立っている時
	if (isHide_) {
		if (remainBulletCount_ < kBulletRimit_) {
			remainBulletCount_++;
		}

		if (position.y >= -7.5f) {
			moveVector += { 0, -0.5f, 0 };
		}
	}
	else {
		Attack();

		if (position.y < -5.0f) {
			moveVector += { 0, 0.5f, 0 };
		}
	}

	position += moveVector;

	//ダメージフラグを確認
	if (isDamage_) {
		//ダメージ時にシェイクをセット
		shake_.SetWidthSwing(5.0f);
		shake_.SetIs(true);
	}

	//シェイクフラグを確認
	if (shake_.Is()) {
		//シェイクの出力を、自機の位置に加算
		shake_.Update();

		shakePos = position;
		shakePos += shake_.GetOutput();
	}

	rot += rotVector;

	// 座標の回転を反映
	Object3d::SetRotation(rot);

	// 座標の変更を反映
	Object3d::SetPosition(shakePos);

	Object3d::Update();

	//シェイク前の座標をセットしなおし
	Object3d::SetPosition(position);

	//ライフ0でデスフラグ
	if (life_ <= 0.0f) {
		isDead_ = true;
	}

	spriteReticle_->SetPosition({
		worldTransform3dReticle_.position_.x ,
		worldTransform3dReticle_.position_.y
		});

	spriteReticle_->SetPosition(input_->GetMousePosition());
	spriteReticle_->Update();

	//残弾数ゲージの変動
	bulletGauge_->GetRestSprite()->
		SetColor({ 0.2f,0.7f,0.2f,5.0f });
	bulletGauge_->SetPosition({
		input_->GetMousePosition().x - 64.0f + 16.0f,
		input_->GetMousePosition().y + 16.0f
		});

	bulletGauge_->SetRest(static_cast<float>(remainBulletCount_));
	bulletGauge_->DecisionFluctuation();
	bulletGauge_->SetIsFluct(true);
	bulletGauge_->Update();

	hpGauge_->SetRest(life_);
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

	float textSize = 2.5f;

	if (remainBulletCount_ <= 0) {
		text_->Print("AmmoIsEmpty",
			spriteReticle_->GetPosition().x - (text_->fontWidth_ * 12.0f),
			spriteReticle_->GetPosition().y,
			textSize);
	}
}

void Player::Draw() {
	Object3d::Draw(worldTransform_);
}

void Player::DrawUI() {
	//	bulletGauge_->Draw();
	hpGauge_->Draw();

	spriteReticle_->Draw();
	text_->DrawAll();
}

void Player::DrawImgui() {
	debugPos_[0] = { GetPosition().x };
	debugPos_[1] = { GetPosition().y };
	debugPos_[2] = { GetPosition().z };

	debugDir_[0] = { GetRotation().x };
	debugDir_[1] = { GetRotation().y };
	debugDir_[2] = { GetRotation().z };

	float hide = isHide_;

	ImGui::Begin("Player");
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(500, 100));
	ImGui::SliderFloat3(
		"PlayerPos", debugPos_, -PosRange_, PosRange_);
	ImGui::SliderFloat3(
		"PlayerDir", debugDir_, 0, DirRange_);
	ImGui::InputFloat("PlayerLife", &life_);
	ImGui::InputFloat("IsHide", &hide);
	ImGui::InputInt("PlayerRemainBullet", &remainBulletCount_);
	ImGui::End();
}

void Player::Finalize() {
	SafeDelete(spriteReticle_);
	bulletGauge_->Finalize();
	hpGauge_->Finalize();
	SafeDelete(text_);
}

void Player::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;

	isDamage_ = true;
}

void Player::Reticle() {
	Corsor cursor{};

	cursor.SetDistance(70.0f + 30.0f);

	//マウスカーソルから、3D照準座標を取得する
	worldTransform3dReticle_.position_ =
		cursor.Get3DRethiclePosition(camera_);

	worldTransform3dReticle_.UpdateMatrix();
}

void Player::Attack() {
	//残弾数が切れていなければ発射可能
	if (remainBulletCount_ > 0) {
		//発射操作を確認
		if (input_->PressMouse(0)) {

			if (bulletCooltime_ <= 0) {


				//弾スピード
				const float kBulletSpeed = 10.0f;
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

				newBullet->SetModel(bulletModel_);

				newBullet->SetScale(worldTransform_.scale_);
				newBullet->SetRotation(worldTransform_.rotation_);
				newBullet->SetPosition(Vector3{
					worldTransform_.matWorld_.m[3][0],
					worldTransform_.matWorld_.m[3][1],
					worldTransform_.matWorld_.m[3][2]
					});

				newBullet->SetVelocity(bulletVelocity);

				newBullet->SetDamage(kGunDamage_);

				newBullet->SetCamera(camera_);

				newBullet->SetGameScene(gameScene_);

				newBullet->Update();

				gameScene_->AddPlayerBullet(std::move(newBullet));

				//発射済みの弾数を一つカウント
				remainBulletCount_--;

				bulletCooltime_ = kDefaultBulletCooltime_;
			}
			else {
				bulletCooltime_--;
			}
		}
	}
}
