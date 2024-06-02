/*プレイヤー*/

#include "Player.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include "GamePlayScene.h"
#include "PlayerBullet.h"
#include <Framework.h>

#include "ObjectManager.h"

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
	float kCenterAnchorPoint_ = 0.5f;

	Character::SetGroupName(groupName_);
	Character::SetDefaultLife(kDefaultLife_);
	Character::SetDefaultPosition({ kDefaultPosX_,kDefaultPosY_,kDefaultPosZ_
		});
	Character::SetSpeed(0.5f);

	Character::Initialize();

	modelActive_ = model_;
	modelHide_ = model_;

	Character::collider_->SetAttribute(COLLISION_ATTR_PLAYER);

	worldTransform3dReticle_ = new WorldTransform();
	worldTransform3dReticle_->Initialize();

	spriteReticle_ = std::make_unique<Sprite>();
	spriteReticle_->Initialize(Framework::kCursorTextureIndex_);
	spriteReticle_->SetAnchorPoint({
		kCenterAnchorPoint_, kCenterAnchorPoint_ });
	spriteReticle_->SetPosition({ WinApp::Win_Width / 2, WinApp::Win_Height / 2 });
	spriteReticle_->SetSize({ 64,64 });

	cursor_ = std::make_unique<Cursor>();
	cursor_->SetDistance(70.0f);
	cursor_->Initialize();

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
	Gauge* newHpGauge = new Gauge();
	newHpGauge->SetRestMax(kDefaultLife_);
	newHpGauge->SetRest(kDefaultLife_);
	newHpGauge->SetMaxTime(kMaxTimeHP_);
	newHpGauge->Initialize();
	newHpGauge->SetPosition({ 64,64 });
	newHpGauge->SetSize({ 1,1 });
	Character::SetHPGauge(newHpGauge);
#pragma endregion

#pragma region カート
	Cart* newCart = Cart::Create();
	newCart->Initialize();
	newCart->SetModel(
		ObjectManager::GetInstance()->
		GetModel(
			ObjectManager::cartModel_
		)
	);
	newCart->SetCamera(camera_);
	Character::SetCart(newCart);
#pragma endregion

#pragma region 残弾数スプライト
	Gauge* newGauge = new Gauge();
	newGauge->Initialize();
	newGauge->GetRestSprite()->SetColor({ 0.2f,0.7f,0.2f,0.5f });
	newGauge->SetRestMax(static_cast<float>(remainBulletCount_));
	newGauge->SetRest(static_cast<float>(remainBulletCount_));
	newGauge->SetMaxTime(maxTimeBullet_);
	newGauge->SetPosition({ 64,64 });
	newGauge->SetSize({ 0.5f,0.5f });
	newGauge->GetAmountSprite()->SetColor({ 0.7f,0.7f,0.2f,0.5f });
	newGauge->Update();
	bulletGauge_.reset(newGauge);
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
	Text* newText = new Text();
	textEmpty_.reset(newText);
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
	Vector2 end{ 64 * 5,64 };

	setupHpGaugePos_.Reset(
		Ease::In_,
		Character::GetTimerMax(),
		ConvertVector2ToVector3(start),
		ConvertVector2ToVector3(end)
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
		if (isPhaseAdvance_) {
			PhaseChange();
		}

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
			const float hidePosY = cartPosYLock_ + 3.0f;

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

		moveEase.Update();
		rotaEase.Update();
		rotVector = rotaEase.GetReturn();
		moveVector = moveEase.GetReturn();

		//position += moveVector;
		//rot += rotVector;

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

		if (countAdv_ >= 100) {
			//あとで自機側で、この向きになるよう回転させる
			rotVector = CreateRotationVector(
				{ 0.0f,1.0f,0.0f }, ConvertToRadian(180.0f));
		}

		Character::SetMovePos(moveVector);
		Character::SetMoveRota(rotVector);
		Character::SetShakePos(shakePos);
	}

	if (Character::IsStart()) {
		spriteReticle_->SetIsInvisible(false);
		bulletGauge_->SetIsInvisible(false);

		//ライフ0でデスフラグ
		if (Character::GetLife() <= 0.0f) {
			if (!isSetupOverPositionEase_) {
				GetEndPositionEase().Reset(
					Ease::In_,
					60,
					Character::GetPosition(),
					{
						Character::GetPosition().x,
						Character::GetPosition().y - 60.0f,
						Character::GetPosition().z - 30.0f
					}
				);

				GetEndRotationEase().Reset(
					Ease::In_,
					30,
					Character::GetRotation(),
					{
						Character::GetRotation().x - ConvertToRadian(40.0f),
						Character::GetRotation().y,
						Character::GetRotation().z
					}
				);

				isSetupOverPositionEase_ = true;
				bulletGauge_->SetIsInvisible(true);
				spriteReticle_->SetIsInvisible(true);
			}
		}
	}
	else {
		bulletGauge_->SetIsInvisible(true);
		spriteReticle_->SetIsInvisible(true);
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
	//spriteReticle_->Draw();
	textEmpty_->DrawAll();

	cursor_->Draw();

	Character::DrawUI();
}

void Player::DrawImgui() {

#pragma region 調整項目
	AdjustmentVariables* adjustmentVariables_ = AdjustmentVariables::GetInstance();

	//調整項目の更新
	adjustmentVariables_->Update();
	ApplyAdjustmentVariables();
#pragma endregion

	//Character::DrawImgui();
}

void Player::Finalize() {
	bulletGauge_->Finalize();
	SafeDelete(worldTransform3dReticle_);
	Character::Finalize();
}

void Player::ApplyAdjustmentVariables() {
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

void Player::UpdateReticle(
	const Vector3& lockonTargetPos
	, float distance
) {
		cursor_->SetDistance(distance);

		cursor_->Update();
		worldTransform3dReticle_->position_ = lockonTargetPos;
		//3Dレティクル座標を更新
		worldTransform3dReticle_->UpdateMatrix();


		//レティクルスプライトの位置
		// エネミーのワールド座標をスクリーン座標に変換して設定
	/*	spriteReticle_->SetPosition(
			cursor_->TransFromWorldToScreen(lockonTargetPos)
		);*/
		spriteReticle_->SetPosition(
			cursor_->TransFromWorldToScreen(cursorPos_)
		);
	//---------------
		Vector2 size2d{};
		Vector4 color2d{};
		float alfaRest = 0.0f;
		//ロックオン中か否かでスプライトのサイズを変える
		if (cursor_->IsLockOn()) {
			size2d = { 80,80 };
			color2d = { 0.7f,0.2f,0.2f,0.8f };
			alfaRest = 0.2f;
		}
		else {
			size2d = { 64,64 };
			color2d = { 1,1,1,1 };
			alfaRest = 0.5f;
		}

		spriteReticle_->SetSize(size2d);
		spriteReticle_->SetColor(color2d);
		spriteReticle_->Update();
		//---------------
			//残弾数ゲージの変動
		bulletGauge_->GetRestSprite()->
			SetColor({ 0.2f,0.7f,0.2f,alfaRest });
		bulletGauge_->SetPosition({
			spriteReticle_->GetPosition().x - 64.0f + 16.0f,
			spriteReticle_->GetPosition().y + 32.0f
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
}

void Player::Attack() {
	//残弾数が切れていなければ発射可能
	if (remainBulletCount_ > 0) {
		//発射操作を確認
		if (input_->TriggerMouse(0)) {
			if (bulletCooltime_ <= 0) {
				//弾スピード
				const float kBulletSpeed = 10.5f;
				//毎フレーム弾が前進する速度
				Vector3 bulletVelocity = { 0.0f,0.0f,kBulletSpeed };

				//速度ベクトルを自機の向きに合わせて回転させる
				//bulletVelocity = Vector3CrossMatrix4(bulletVelocity, worldTransform_.matWorld_);
				bulletVelocity =
					worldTransform3dReticle_->posWorld_
					- Character::GetPosWorld();

				bulletVelocity = Vector3Normalize(bulletVelocity);
				bulletVelocity *= kBulletSpeed;

				Character::SetBulletDamage(kGunDamage_);
				Character::SetBulletVelocity(bulletVelocity);

				Character::Attack();

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

void Player::PhaseChange() {
	Vector3 startPos{};
	Vector3 endPos{};

	Vector3 startRota{};
	Vector3 endRota{};
		float updateRota = ConvertToRadian(180.0f);
		if (countAdv_ % 2 == 1) {
			updateRota *= -1;
		}
		if(countAdv_ == 1) {

			startPos = Character::GetPosition();
			endPos = {
				Character::GetPosition().x,
				Character::GetPosition().y,
				Character::GetPosition().z +50.0f
			};

			startRota = easeRotaS_ ;
			endRota = {
				Character::GetRotation().x,
				+ updateRota / 2,
				Character::GetRotation().z
			};
		}
	else if (countAdv_ < cBossPhaseIndex_) {
		//countAdv_++;

			startPos = Character::GetPosition();
			endPos = {
				Character::GetPosition().x,
				Character::GetPosition().y,
				Character::GetPosition().z +50.0f
			};

		startRota = easeRotaS_ ;
		endRota = {
			Character::GetRotation().x,
			Character::GetRotation().y + updateRota,
			Character::GetRotation().z
		};
	}
	else {

			startPos = Character::GetPosition();
			endPos = {
				Character::GetPosition().x,
				Character::GetPosition().y,
				Character::GetPosition().z +50.0f
			};

		startRota = easeRotaS_ ;
		endRota = {
			Character::GetRotation().x,
			0.0f,
			Character::GetRotation().z
		};
	}


	moveEase.Reset(
		Ease::InOut_,
		Character::GetTimerMax(),
		startPos,
		endPos
	);

	rotaEase.Reset(
		Ease::InOut_,
		Character::GetTimerMax(),
		startRota,
		endRota
	);

	isPhaseAdvance_ = false;

	easeRotaS_ = endRota;
}
