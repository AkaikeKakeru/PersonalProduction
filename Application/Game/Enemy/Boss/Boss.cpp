#include "Boss.h"

#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "SphereCollider.h"

#include <cassert>
#include <ObjectManager.h>

#include "GamePlayScene.h"
#include <Framework.h>

#include <Quaternion.h>
#include <Ease.h>

#include <SafeDelete.h>
#include <Random.h>

#ifdef _DEBUG
#include <imgui.h>
#endif

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
	Character::SetScale(Object3d::GetScale());
	Character::SetSpeed(0.2f);

	const int timerMax = 50;

	moveEase_.Reset(
		Ease::InOut_,
		timerMax,
		Character::GetPosition(),
		{
			kMoveEaseRangeX_,
			kMoveEaseRangeY_,
			kDefaultPosZ_
		}
	);

	Character::Initialize();

	//半径分だけ足元から浮いた座標を球の中心にする
	SetCollider(new SphereCollider(
		Vector3{ 0.0f,radiusCollider_,0.0f },
		radiusCollider_)
	);

	collider_->SetAttribute(COLLISION_ATTR_ENEMYS);

#pragma region HPスプライト
	Gauge* newHpGauge = new Gauge();
	newHpGauge->SetRestMax(kDefaultLife_);
	newHpGauge->SetRest(kDefaultLife_);
	newHpGauge->SetMaxTime(kMaxTimeHP_);
	newHpGauge->Initialize();
	newHpGauge->SetPosition({ 0,32 });
	newHpGauge->SetSize({ 1,0.5f });
	newHpGauge->SetIsInvisible(false);
	Character::SetHPGauge(newHpGauge);
#pragma endregion

	//ResetWeak();

#pragma region カート

	Cart* newCart = Cart::Create();
	newCart->Initialize();
	newCart->SetModel(
		ObjectManager::GetInstance()->
		GetModel(
			ObjectManager::cartModel_
		)
	);
	newCart->SetPosition({ 0.0f,-200.0f,0.0f });
	newCart->SetCamera(camera_);
	Character::SetCart(newCart);

#pragma endregion

	//Character::Update();

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

	return true;
}

void Boss::Update() {
	if (weakPoint_.size() <= 0) {
		isBreak_ = true;
	}

	//ブレイクフラグの立った弱点を削除
	weakPoint_.remove_if([](std::unique_ptr<WeakPoint>& weak) {
		return weak->IsBreak();
		});

	// 現在の座標を取得
	Vector3 positionBoss = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rotaBoss = Object3d::GetRotation();

	Vector3 updatePos{};
	Vector3 updateRota{};

	ResetNextEase();

	if (isMoveBoot_) {
		moveEase_.Update();
		positionBoss = moveEase_.GetReturn();

		rotaEase_.Update();
		rotaBoss = rotaEase_.GetReturn();
	}
	else {

		stayTimer_--;

		if (stayTimer_ <= 0) {
			isMoveBoot_ = true;

			stayTimer_ = kStayTimer_;
		}
	}

	Character::SetMovePos(positionBoss);
	Character::SetMoveRota(rotaBoss);

	if (isActive_) {
		////ブレイクフラグの立った弱点を削除
		//weakPoint_.remove_if([](std::unique_ptr<WeakPoint>& weak) {
		//	return weak->IsBreak();
		//	});

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

				for (std::unique_ptr<WeakPoint>& weak : weakPoint_) {
					weak->SetIsBreak(true);
				}
			}
		}
	}
	else {
		//isAttack_ = false;

		waitTimer_--;

		//攻撃タイマーの再設定
		if (waitTimer_ <= 0) {
			ResetWeak();
			isBreak_ = false;

			//攻撃タイマーを初期化
			attackTimer_ = kFireInterval;
			waitTimer_ = kWaitInterval;

			isActive_ = true;
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
	Matrix4 matViewPort = Matrix4Identity();
	matViewPort.m[0][0] = static_cast<float>(WinApp::Win_Width) / 2;
	matViewPort.m[1][1] = static_cast<float>(-(WinApp::Win_Height)) / 2;
	matViewPort.m[3][0] = static_cast<float>(WinApp::Win_Width) / 2;
	matViewPort.m[3][1] = static_cast<float>(WinApp::Win_Height) / 2;

	Matrix4 matVPV = camera_->GetViewMatrix()
		* camera_->GetProjectionMatrix()
		* matViewPort;
	Character::GetHPGauge()->GetRestSprite()->
		SetColor({ 0.2f,0.7f,0.2f,5.0f });
	Character::GetHPGauge()->DecisionFluctuation();
	Character::GetHPGauge()->SetIsFluct(true);
	Character::GetHPGauge()->SetIsInvisible(false);

	Character::GetHPGauge()->SetPosition({
		WinApp::Win_Width / 2, 20.0f
		});

	float l = Character::GetLife();
	l -= 0.1f;
	Character::SetLife(l);

	Character::Update();


	for (std::unique_ptr<WeakPoint>& weak : weakPoint_) {
		int32_t now = 0;

	//	if (weak) {
			weak->SetPosition(Object3d::GetPosition());
			weak->Update();
	//	}

		weakPosContainer_[now] = weak->GetPosition();

		now++;
	}


}

void Boss::Draw() {
	Character::Draw();
	for (std::unique_ptr<WeakPoint>& weak : weakPoint_) {
		if (weak) {
			weak->Draw();
		}
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
	Character::OnCollision(info);
}

void Boss::StartMove() {
	Character::GetHPGauge()->SetIsInvisible(false);

	Character::StartMove();
}

void Boss::OverMove() {
	Character::OverMove();
}

void Boss::Attack() {
//	isAttack_ = true;

	//弾スピード
	const float kBulletSpeed = 6.0f;

	Vector3 worldPos =
		worldTransform_.posWorld_;

	Vector3 worldPosPlayer =
		//player_->GetPosWorld();

		targetPos_;

	//毎フレーム弾が前進する速度
	Vector3 bulletVelocity = worldPosPlayer - worldPos;

	bulletVelocity = Vector3Normalize(bulletVelocity);

	bulletVelocity *= kBulletSpeed;

	Character::SetBulletDamage(kGunDamage_);
	Character::SetBulletVelocity(bulletVelocity);

	Character::Attack();
}

void Boss::ResetWeak() {
	assert(camera_);

	for (int i = 0; i < kWeakPointCount_; i++) {

		//弱点位置初期化用
		std::unique_ptr<WeakPoint> newWeak =
			std::make_unique<WeakPoint>();

		newWeak->Initialize();
		newWeak->SetModel(
			ObjectManager::GetInstance()->
			GetModel(ObjectManager::bulletModel_)
		);

		newWeak->SetScale((worldTransform_.scale_));
		newWeak->SetRotation(worldTransform_.rotation_);

		newWeak->SetIsBreak(false);

		newWeak->SetCamera(camera_);
		newWeak->GetCollider()->SetAttribute(
			COLLISION_ATTR_ENEMYS
			//Character::GetCollider()->GetAttribute()
		);

		Vector3 newWeakPos{};

		Vector3 pos = worldTransform_.position_;

		//弱点位置オフセット(Vector3)
		Vector3 offsetVec3{};
		//弱点位置オフセット(Int)
		float offsetFloat = 10.0f;

		switch (i) {
		case 0:
			offsetVec3.x = -offsetFloat;
			offsetVec3.y = offsetFloat;
			break;
		case 1:
			offsetVec3.x = -offsetFloat;
			offsetVec3.y = -offsetFloat;
			break;
		case 2:
			offsetVec3.x = offsetFloat;
			offsetVec3.y = offsetFloat;
			break;
		case 3:
			offsetVec3.x = offsetFloat;
			offsetVec3.y = -offsetFloat;
			break;

		default:
			break;
		}

		newWeak->SetOffset(offsetVec3);

		newWeak->SetPosition(newWeakPos);

		newWeak->Update();

		weakPoint_.push_back(std::move(newWeak));
	}
}

void Boss::ResetNextEase() {

	//横移動イーズを、エンドフラグを起点にリセット
	if (moveEase_.IsEnd()) {
		Vector3 rand{};
		while (true) {

			rand = {
				RandomOutputFloat(-1.0f, 1.0f),
				RandomOutputFloat(-1.0f, 1.0f),
				RandomOutputFloat(-1.0f, 0.0f)
			};

			//-1.0f or 0.0f or 1.0f　のどれかにする。
			SortingSign(rand.x);
			SortingSign(rand.y);
			SortingSign(rand.z);

			//前回と同じ出力なら抽選し直し
			if (moveEaseVec_.x != rand.x
				|| moveEaseVec_.y != rand.y
				|| moveEaseVec_.z != rand.z) {
				break;
			}
		}

#pragma region 横移動イーズのリセット
		Vector3 endMovePos = {
			kMoveEaseRangeX_ * moveEaseVec_.x,
			kMoveEaseRangeY_ * moveEaseVec_.y,
			kMoveEaseRangeZ_ * moveEaseVec_.z,
		};

		moveEase_.Reset(
			Ease::InOut_,
			kMoveEaseTimerMax_,
			Character::GetPosition(),
			endMovePos
		);
#pragma endregion

#pragma region 回転イーズのリセット
		Vector3 axisZ = Vector3Normalize(targetPos_ - endMovePos);
		Vector3 axisX = Vector3Normalize(Vector3Cross({ 0.0f,1.0f,0.0f }, axisZ));
		Vector3 axisY = Vector3Cross(axisZ, axisX);

		//回転行列
		Matrix4 matRot = {
			axisX.x,axisX.y,axisX.z,0,
			axisY.x,axisY.y,axisY.z,0,
			axisZ.x,axisZ.y,axisZ.z,0,
			0,0,0,1
		};

		rotaEase_.Reset(
			Ease::InOut_,
			kMoveEaseTimerMax_,
			Character::GetRotation(),
			{
				-Vector3TransformCoord(axisZ, matRot).y,
				Vector3TransformCoord(axisZ, matRot).x,
				0.0f
			}
		);
#pragma endregion
		moveEaseVec_ = rand;

		isMoveBoot_ = false;
	}
}

void Boss::SortingSign(float& f) {
	if (f < -0.7f) {
		f = -1.0f;
	}
	else if (f > 0.3f) {
		f = 1.0f;
	}
	else {
		f = 0.0f;
	}
}
