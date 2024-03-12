#include "Boss.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include <cassert>
#include <ObjectManager.h>

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
	Character::SetScale({
		20,
		20,
		20
		});
	Character::SetSpeed(0.2f);

	Character::Initialize();

	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

#pragma region HPスプライト
	Gauge* newHpGauge = new Gauge();
	newHpGauge->SetRestMax(kDefaultLife_);
	newHpGauge->SetRest(kDefaultLife_);
	newHpGauge->SetMaxTime(kMaxTimeHP_);
	newHpGauge->Initialize();
	newHpGauge->SetPosition({ 64,64 });
	newHpGauge->SetSize({ 1,0.5f });
	Character::SetHPGauge(newHpGauge);
#pragma endregion

	ResetWeak();

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

	return false;
}

void Boss::Update() {
	//ブレイクフラグの立った弱点を削除
	weakPoint_.remove_if([](std::unique_ptr<WeakPoint>& weak) {
		return weak->IsBreak();
		});

	if (weakPoint_.size() <= 0) {
		isBreak_ = true;
	}

	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

	Character::SetMovePos(position);
	Character::SetMoveRota(rot);

	if (isActive_) {
		if (isBreak_) {
			isActive_ = false;
		}
		else {
			//攻撃タイマーを減らしていく
			attackTimer_--;

			//攻撃タイマー0で発射処理
			if (attackTimer_ <= 0) {
				//攻撃
				Attack();
				isActive_ = false;
			}
		}
	}
	else {
		isAttack_ = false;

		waitTimer_--;

		//攻撃タイマーの再設定
		if (waitTimer_ <= 0) {
			ResetWeak();
			isBreak_ = false;

			//攻撃タイマーを初期化
			attackTimer_ = kFireInterval;
			waitTimer_ = kWaitInterval;
		}
	}

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

	Character::Update();

	for (std::unique_ptr<WeakPoint>& weak : weakPoint_) {
		Vector3 updatePos = weak->GetPosition();

		updatePos += position;

		weak->SetPosition(updatePos);

		weak->Update();
	}
}

void Boss::Draw() {
	Character::Draw();
	for (std::unique_ptr<WeakPoint>& weak : weakPoint_) {
		weak->Draw();
	}
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
	for (std::unique_ptr<WeakPoint>& weak : weakPoint_) {
		weak->OnCollision(info);
	}
	Character::OnCollision(info);
}

void Boss::StartMove() {
	Character::StartMove();
}

void Boss::OverMove() {
	Character::OverMove();
}

void Boss::Attack() {
	isAttack_ = true;
}

void Boss::ResetWeak() {
	assert(camera_);

	for (int i = 0; i < kWeakPointCount_; i++) {

	//弱点位置初期化用
	std::unique_ptr<WeakPoint> newWeak =
		std::make_unique<WeakPoint>();

	newWeak->Initialize();
	newWeak->SetModel(Character::GetBulletModel());

	newWeak->SetScale((worldTransform_.scale_ / 20));
	newWeak->SetRotation(worldTransform_.rotation_);

	newWeak->SetCamera(camera_);
	newWeak->GetCollider()->SetAttribute(Character::GetCollider()->GetAttribute());

	Vector3 newWeakPos{};

	Vector3 pos = worldTransform_.position_;

	//弱点位置オフセット(Vector3)
	Vector3 offsetVec3{};
	//弱点位置オフセット(Int)
	int offsetInt = 10;

		float convertI = (float)i;

		//2点ほどマイナス側に
		if (i < kWeakPointCount_ / 2) {
			offsetVec3 = {
				convertI * offsetInt,
				convertI * offsetInt,
				0
			};
		}
		//2点ほどプラス側に
		else {
			offsetVec3 = {
				(convertI - 2) * -offsetInt,
				(convertI - 2) * -offsetInt,
				0
			};
		};

		newWeakPos =
			pos + offsetVec3;

		newWeak->SetPosition(newWeakPos);

		weakPoint_.push_back(std::move(newWeak));
	}
}
