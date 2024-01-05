/*キャラクター*/

#include "Character.h"

#include "CollisionManager.h"
#include "SphereCollider.h"
#include "CollisionAttribute.h"

#include <Quaternion.h>
#include <Ease.h>

#include <cassert>
#include <SafeDelete.h>

#ifdef _DEBUG
#include <imgui.h>
#endif

#include "AdjustmentVariables.h"

Input* Character::input_ = Input::GetInstance();
CollisionManager* Character::collisionManager_ = CollisionManager::GetInstance();
SpriteBasis* Character::spriteBas_ = SpriteBasis::GetInstance();

Character* Character::Create() {
	//オブジェクトのインスタンスを生成
	Character* instance = new Character();
	if (instance == nullptr) {
		return nullptr;
	}

	return instance;
}

bool Character::Initialize() {
	if (!Object3d::Initialize()) {
		return false;
	}
	//コライダ－追加

	//半径分だけ足元から浮いた座標を球の中心にする
	SetCollider(new SphereCollider(
		Vector3{ 0.0f,radiusCollider_,0.0f },
		radiusCollider_)
	);

	collider_->SetAttribute(COLLISION_ATTR_PLAYER);

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
		radiusCollider_);

	ApplyAdjustmentVariables();
#pragma endregion

#pragma region HPスプライト
	hpGauge_ = new Gauge();
	hpGauge_->Initialize();

	hpGauge_->SetRestMax(life_);
	hpGauge_->SetRest(life_);
	hpGauge_->SetMaxTime(maxTimeHP_);

	hpGauge_->SetPosition({ 64,64 });
	hpGauge_->SetSize({ 1,1 });
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

void Character::Update() {
	//camera_->Update();

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

	if (isStart_) {
		// 座標の回転を反映
		Object3d::SetRotation(moveRota_);

		// 座標の変更を反映
		Object3d::SetPosition(shakePos_);

		Object3d::Update();

		//シェイク前の座標をセットしなおし
		Object3d::SetPosition(movePos_);
	}
	else {
		if (!isOver_) {
			StartMove();
		}
	}

	if (isStart_) {

		//ライフ0でデスフラグ
		if (life_ <= 0.0f) {
			isDead_ = true;
		}

		hpGauge_->SetRest(life_);
		//通常は緑、ピンチで赤
		if (life_ < defaultLife_ / 2) {
			hpGauge_->GetRestSprite()->
				SetColor({ 0.7f,0.2f,0.2f,1.0f });
		}
		else {
			hpGauge_->GetRestSprite()->
				SetColor({ 0.2f,0.7f,0.2f,1.0f });
		}
	}

	if (isDead_) {
		OverMove();
	}

	hpGauge_->Update();
}

void Character::Draw() {
	Object3d::Draw(worldTransform_);
}

void Character::DrawUI() {
	hpGauge_->Draw();
}

void Character::DrawImgui() {
	const char* cGroupName = groupName_;

	debugPos_[0] = { GetPosition().x };
	debugPos_[1] = { GetPosition().y };
	debugPos_[2] = { GetPosition().z };

	debugDir_[0] = { GetRotation().x };
	debugDir_[1] = { GetRotation().y };
	debugDir_[2] = { GetRotation().z };

#pragma region 調整項目
	AdjustmentVariables* adjustmentVariables_ = AdjustmentVariables::GetInstance();

	//調整項目の更新
	adjustmentVariables_->Update();
	ApplyAdjustmentVariables();
#pragma endregion

#ifdef _DEBUG
	ImGui::Begin(cGroupName);
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(500, 100));
	ImGui::SliderFloat3(
		"Pos", debugPos_, -PosRange_, PosRange_);
	ImGui::SliderFloat3(
		"Dir", debugDir_, 0, DirRange_);
	ImGui::InputFloat("Life", &life_);
	ImGui::End();
#endif
}

void Character::Finalize() {
	hpGauge_->Finalize();
}

void Character::ApplyAdjustmentVariables() {
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

	radiusCollider_ =
		adjustmentVariables_->GetFloatValue(
			groupName_, "RadiusCollider"
		);
}

void Character::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;

	isDamage_ = true;
}

void Character::StartMove() {
	Vector3 move{};
	Vector2 start{ -500,64 };
	Vector2 end{ 64,64 };

	startEase1_.Update();
	move = startEase1_.GetReturn();

	hpGauge_->SetPosition(
		ConvertVector3ToVector2(move)
	);

	hpGauge_->Update();

	move = {};

	startEase2_.Update();
	move = startEase2_.GetReturn();

	Object3d::SetPosition(move);

	Object3d::Update();

	if (startEase2_.IsEnd()) {
		Object3d::SetPosition(defaultPos_);

		isStart_ = true;
	}
}

void Character::OverMove() {
	Vector3 move{};
	move = {};

	endEase1_.Update();
	endEase2_.Update();

	move = endEase1_.GetReturn();

	Object3d::SetPosition(move);

	move = {};
	move = endEase2_.GetReturn();

	move = {
		ConvertToRadian(move.x),
		ConvertToRadian(move.y),
		ConvertToRadian(move.z) };

	Object3d::SetRotation(move);

	Object3d::Update();

	if (endEase1_.IsEnd()) {
		isOver_ = true;
	}
}
