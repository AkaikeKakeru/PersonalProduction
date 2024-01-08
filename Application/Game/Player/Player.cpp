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
	Character::SetDefaultPosition({ kDefaultPosX_,kDefaultPosY_,kDefaultPosZ_
		});
	Character::SetSpeed(0.5f);

	Character::Initialize();

	modelActive_ = model_;
	modelHide_ = model_;

	collider_->SetAttribute(COLLISION_ATTR_PLAYER);

	worldTransform3dReticle_.Initialize();

	spriteReticle_ = new Sprite();
	spriteReticle_->Initialize(Framework::kCursorTextureIndex_);

	spriteReticle_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteReticle_->SetSize({ 64,64 });

#pragma region 調整項目
	AdjustmentVariables* adjustmentVariables_ = AdjustmentVariables::GetInstance();

	//グループ追加
	AdjustmentVariables::GetInstance()->CreateGroup(groupName_);

	adjustmentVariables_->AddItem(
		groupName_, "Position",
		worldTransform_.position_);

	adjustmentVariables_->AddItem(
		groupName_, "Rotation",
		worldTransform_.rotation_);

	adjustmentVariables_->AddItem(
		groupName_, "Scale",
		worldTransform_.scale_);

	adjustmentVariables_->AddItem(
		groupName_, "RadiusCollider",
		Character::GetRadius());

	ApplyAdjustmentVariables();
#pragma endregion

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

#ifdef _DEBUG
	{
		float debugPos[3] = {
			GetPosition().x,
			GetPosition().y,
			GetPosition().z
		};

		Character::SetDebugPosition(debugPos);

		float debugDir[3] = {
			GetRotation().x,
			GetRotation().y,
			GetRotation().z
		};

		Character::SetDebugDirection(debugDir);
	}
#endif // _DEBUG

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

	setupHpGaugePos_.Reset(
		Ease::In_,
		Character::GetTimerMax(),
		ConvertVector2ToVector3(start),
		ConvertVector2ToVector3(end)
	);

	Character::GetStartPositionEase().Reset(
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
	Character::GetStartRotationEase().Reset(
		Ease::In_,
		Character::GetTimerMax(),
		Object3d::GetRotation(),
		Object3d::GetRotation()
	);

	Character::GetEndPositionEase().Reset(
		Ease::In_,
		60,
		Object3d::GetPosition(),
		{
			Object3d::GetPosition().x,
			Object3d::GetPosition().y - 100.0f,
			Object3d::GetPosition().z
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

	return true;
}

void Player::Update() {
	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

	//移動ベクトル
	Vector3 moveVector = { 0.0f,0.0f,0.0f };
	//回転ベクトル
	Vector3 rotVector = { 0.0f,0.0f,0.0f };

	//シェイク後座標
	Vector3 shakePos = Object3d::GetPosition();

	if (Character::IsStart()) {
		//入力で隠れフラグ操作
		if (input_->PressMouse(1)) {
			Object3d::SetModel(modelHide_);
			isHide_ = true;
		}
		else {
			Object3d::SetModel(modelActive_);
			isHide_ = false;
		}

		//隠れフラグが立っている時
		if (isHide_) {
			if (isHideOld_ == false) {
				cartPosYLock_ = worldTransform_.matWorld_.m[3][1] + kConfigCartPosY_;
			}
			const float hidePosY = cartPosYLock_ + 2.0f;


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
			if (!isSetupOverPositionEase_) {
				GetEndPositionEase().Reset(
					Ease::In_,
					60,
					Object3d::GetPosition(),
					{
						Object3d::GetPosition().x,
						Object3d::GetPosition().y - 60.0f,
						Object3d::GetPosition().z - 30.0f
					}
				);

				GetEndRotationEase().Reset(
					Ease::In_,
					30,
					Object3d::GetRotation(),
					{
						Object3d::GetRotation().x - ConvertToRadian(40.0f),
						Object3d::GetRotation().y,
						Object3d::GetRotation().z
					}
				);

				isSetupOverPositionEase_ = true;
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

#ifdef _DEBUG
	{

		position = {
			Character::GetDebugPosition()[0],
			Character::GetDebugPosition()[1],
			Character::GetDebugPosition()[2]
		};

		rot = {
			Character::GetDebugDirection()[0],
			Character::GetDebugDirection()[1],
			Character::GetDebugDirection()[2]
		};
	}
#endif // _DEBUG

	if (IsHide()) {

	Character::GetCart()->SetPosition(
		Vector3{
			worldTransform_.matWorld_.m[3][0],
			cartPosYLock_,
			worldTransform_.matWorld_.m[3][2]
		}
	);
	}
	else {
		Character::GetCart()->SetPosition(
			Vector3{
				worldTransform_.matWorld_.m[3][0],
				worldTransform_.matWorld_.m[3][1] + kConfigCartPosY_,
				worldTransform_.matWorld_.m[3][2]
			}
		);
	}

	Character::Update();
	if (IsHide()) {
		isHideOld_ = true;
	}
	else {
		isHideOld_ = false;
	}
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

#pragma region 調整項目
	AdjustmentVariables* adjustmentVariables_ = AdjustmentVariables::GetInstance();

	//調整項目の更新
	adjustmentVariables_->Update();
	ApplyAdjustmentVariables();
#pragma endregion

	Character::DrawImgui();
}

void Player::Finalize() {
	SafeDelete(spriteReticle_);
	bulletGauge_->Finalize();
	SafeDelete(textEmpty_);

	Character::Finalize();
}

void Player::ApplyAdjustmentVariables() {
	//Character::ApplyAdjustmentVariables();

	AdjustmentVariables* adjustmentVariables_ = AdjustmentVariables::GetInstance();

	worldTransform_.position_ =
		adjustmentVariables_->GetVector3Value(
			groupName_, "Position"
		);

	Vector3 rota =
		adjustmentVariables_->GetVector3Value(
			groupName_, "Rotation"
		);

	worldTransform_.rotation_ = CreateRotationVector(
		rota, ConvertToRadian(0.0f)
	);

	worldTransform_.scale_ =
		adjustmentVariables_->GetVector3Value(
			groupName_, "Scale"
		);

	Character::SetRadius(
		adjustmentVariables_->GetFloatValue(
			groupName_, "RadiusCollider"
		)
	);
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
	Vector3 move{};
	setupHpGaugePos_.Update();
	move = setupHpGaugePos_.GetReturn();

	Character::GetHPGauge()->SetPosition(
		ConvertVector3ToVector2(move)
	);

	Character::GetHPGauge()->Update();

	Character::StartMove();
}

void Player::OverMove() {
	Character::OverMove();
}
