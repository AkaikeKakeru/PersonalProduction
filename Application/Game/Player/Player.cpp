/*プレイヤー*/

#include "Player.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include "GamePlayScene.h"
#include "PlayerBullet.h"
#include <Framework.h>

#include <Quaternion.h>
#include <Ease.h>

#include <cassert>
#include <SafeDelete.h>

#ifdef _DEBUG
#include <imgui.h>
#endif

#include "AdjustmentVariables.h"

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

	Character::Create();

	return instance;
}

bool Player::Initialize() {
	Character::SetGroupName(groupName_);
	Character::SetDefaultLife(kDefaultLife_);
	Character::SetDefaultPosition({
		kDefaultPosX_,kDefaultPosY_,kDefaultPosZ_
		});

	Character::Initialize();

	collider_->SetAttribute(COLLISION_ATTR_PLAYER);

	worldTransform3dReticle_.Initialize();

	spriteReticle_ = new Sprite();
	spriteReticle_->Initialize(Framework::kCursorTextureIndex_);

	spriteReticle_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteReticle_->SetSize({ 64,64 });

#pragma region HPスプライト
	Character::GetHPGauge()->SetRestMax(kDefaultLife_);
	Character::GetHPGauge()->SetRest(kDefaultLife_);
	Character::GetHPGauge()->SetMaxTime(kMaxTimeHP_);

	Character::GetHPGauge()->SetPosition({ 64,64 });
	Character::GetHPGauge()->SetSize({ 1,1 });
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

	bulletGauge_->GetAmountSprite()->SetColor({ 0.7f,0.7f,0.2f,0.5f });

	bulletGauge_->Update();
#pragma endregion

	//テキスト
	textEmpty_ = new Text();
	textEmpty_->Initialize(Framework::kTextTextureIndex_);
	textEmpty_->SetString("AmmoIsEmpty");

	float len = (float)textEmpty_->GetString().length() + 1.0f;

	textEmpty_->SetPosition(
		{
			spriteReticle_->GetPosition().x - (textEmpty_->fontWidth_ * len),
			spriteReticle_->GetPosition().y
		}
	);
	textEmpty_->SetSize({ 2.5f,2.5f });

	shake_.SetWidthSwing(5.0f);

	Vector3 move{};
	bulletGauge_->SetPosition({ 64,64 });

	Vector2 start{ -500,64 };
	Vector2 end{ 64,64 };

	Character::GetStartEase1().Reset(
		Ease::In_,
		Character::GetTimerMax(),
		ConvertVector2ToVector3(start),
		ConvertVector2ToVector3(end)
	);

	Character::GetStartEase2().Reset(
		Ease::In_,
		Character::GetTimerMax(),
		{
			0.0f,
			Character::GetDefaultPosition().y,
			0.0f
		},
		{
			0.0f,
			Character::GetDefaultPosition().y,
			Character::GetDefaultPosition().z
		}
		);

	Character::GetEndEase1().Reset(
		Ease::In_,
		60,
		Object3d::GetPosition(),
		{
			Object3d::GetPosition().x,
			Object3d::GetPosition().y - 100.0f,
			Object3d::GetPosition().z
		}
	);

	Character::GetEndEase1().Reset(
		Ease::In_,
		60,
		Object3d::GetPosition(),
		{
			Object3d::GetPosition().x,
			Object3d::GetPosition().y - 100.0f,
			Object3d::GetPosition().z
		}
	);

	return true;
}

void Player::Update() {
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

	//シェイク後座標
	Vector3 shakePos = Object3d::GetPosition();

	if (Character::IsStart()) {
		//入力で隠れフラグ操作
		if (input_->PressMouse(1)) {
			isHide_ = true;
		}
		else {
			isHide_ = false;
		}

		//隠れフラグが立っている時
		if (isHide_) {
			const float hidePosY = -7.5f;

			if (remainBulletCount_ < kBulletRimit_) {
				remainBulletCount_++;
			}

			if (position.y >= hidePosY) {
				moveVector += { 0, -Character::GetSpeed(), 0 };
			}
		}
		else {
			Attack();

			if (position.y < Character::GetDefaultPosition().y) {
				moveVector += { 0, Character::GetSpeed(), 0 };
			}
		}

		position += moveVector;

		//ダメージフラグを確認
		if (Character::IsDamage()) {
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

		Character::SetMovePos(position);
		Character::SetMoveRota(rot);
		Character::SetShakePos(shakePos);
	}

	if (Character::IsStart()) {
		//ライフ0でデスフラグ
		if (Character::GetLife() <= 0.0f) {
			if (!Character::IsDead()) {
				GetEndEase1().Reset(
					Ease::In_,
					60,
					Object3d::GetPosition(),
					{
						Object3d::GetPosition().x,
						Object3d::GetPosition().y - 100.0f,
						Object3d::GetPosition().z - 20.0f
					}
				);

				GetEndEase2().Reset(
					Ease::In_,
					30,
					Object3d::GetRotation(),
					{
						Object3d::GetRotation().x,
						40.0f,
						Object3d::GetRotation().z
					}
				);
			}
		}
	}

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

	if (remainBulletCount_ <= 0) {
		float len = (float)textEmpty_->GetString().length() + 1.0f;
		textEmpty_->SetPosition(
			{
				spriteReticle_->GetPosition().x - (textEmpty_->fontWidth_ * len),
				spriteReticle_->GetPosition().y
			}
		);
		textEmpty_->Print();
	}

	Character::Update();
}

void Player::Draw() {
	Character::Draw();
}

void Player::DrawUI() {
	bulletGauge_->Draw();
	spriteReticle_->Draw();
	textEmpty_->DrawAll();

	Character::DrawUI();
}

void Player::DrawImgui() {
	Character::DrawImgui();
}

void Player::Finalize() {
	SafeDelete(spriteReticle_);
	bulletGauge_->Finalize();
	SafeDelete(textEmpty_);

	Character::Finalize();
}

void Player::ApplyAdjustmentVariables() {
	Character::ApplyAdjustmentVariables();
}

void Player::OnCollision(const CollisionInfo& info) {
	Character::OnCollision(info);
}

void Player::UpdateReticle(const Vector3& targetWorldPos) {

	//ゲームシーンからカーソルを借りる

	//3Dレティクル座標を更新
	worldTransform3dReticle_.position_ = targetWorldPos;
	worldTransform3dReticle_.UpdateMatrix();

	//レティクルスプライトの位置
	// エネミーのワールド座標をスクリーン座標に変換して設定
	spriteReticle_->SetPosition(
		Character::GetGamePlayScene()->GetCursor()->TransFromWorldToScreen(targetWorldPos)
	);
	//ロックオン中か否かでスプライトのサイズを変える
	if (Character::GetGamePlayScene()->GetCursor()->IsLockOn()) {
		spriteReticle_->SetSize({ 80,80 });
		spriteReticle_->SetColor({ 0.7f,0.2f,0.2f,0.8f });
	}
	else {
		spriteReticle_->SetSize({ 64,64 });
		spriteReticle_->SetColor({ 1,1,1,1 });
	}

	spriteReticle_->Update();
}

void Player::Attack() {
	//残弾数が切れていなければ発射可能
	if (remainBulletCount_ > 0) {
		//発射操作を確認
		if (input_->TriggerMouse(0)) {
			if (bulletCooltime_ <= 0) {
				//弾スピード
				const float kBulletSpeed = 17.5f;
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

				newBullet->SetModel(Character::GetBulletModel());

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

				newBullet->SetGameScene(Character::GetGamePlayScene());

				newBullet->Update();

				Character::GetGamePlayScene()->AddPlayerBullet(std::move(newBullet));

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

void Player::StartMove() {
	Character::StartMove();
}

void Player::OverMove() {
	Character::OverMove();
}
