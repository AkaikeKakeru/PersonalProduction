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
Model* Character::cartModel_ = nullptr;

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

#pragma region HPスプライト
	hpGauge_ = new Gauge();
	hpGauge_->Initialize();

	hpGauge_->SetRestMax(life_);
	hpGauge_->SetRest(life_);
	hpGauge_->SetMaxTime(maxTimeHP_);

	hpGauge_->SetPosition({ 64,64 });
	hpGauge_->SetSize({ 1,1 });
#pragma endregion

	cart_ = new Cart();
	cart_->Initialize();
	cart_->SetModel(cartModel_);
	cart_->SetCamera(camera_);

	return true;
}

void Character::Update() {
	// 現在の座標を取得
	Vector3 position = Object3d::GetPosition();
	// 現在の回転を取得
	Vector3 rot = Object3d::GetRotation();

	if (isStart_) {
		position = Object3d::GetPosition();
		// 座標の回転を反映
		Object3d::SetRotation(moveRota_);

		// 座標の変更を反映
		Object3d::SetPosition(shakePos_);

		Object3d::Update();

		position = Object3d::GetPosition();

		//シェイク前の座標をセットしなおし
		Object3d::SetPosition(movePos_);

		Object3d::Update();

		position = Object3d::GetPosition();
	}
	else {
		if (!isOver_) {
			StartMove();
		}
	}

	if (isStart_) {

		//ライフ0でデスフラグ
		if (life_ <= 0.0f) {
			OverMove();
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

	if (isOver_) {
		SetIsDead(true);
	}

	hpGauge_->Update();

	cart_->Update();
}

void Character::Draw() {
	Object3d::Draw(worldTransform_);
	cart_->Draw();
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
	cart_->Finalize();
	SafeDelete(cart_);
	//SafeDelete(cartModel_);
}

void Character::OnCollision(const CollisionInfo& info) {
	CollisionInfo colInfo = info;

	isDamage_ = true;
}

void Character::StartMove() {
	Vector3 move{};

	startPositionEase_.Update();
	startRotationEase_.Update();

	move = startPositionEase_.GetReturn();
	Object3d::SetPosition(move);

	move = startRotationEase_.GetReturn();

	move = {
		move.x,
		move.y,
		move.z };

	Object3d::SetRotation(move);

	Object3d::Update();

	if (startPositionEase_.IsEnd()) {
		isStart_ = true;
	}
}

void Character::OverMove() {
	Vector3 move{};
	move = {};

	endPositionEase_.Update();
	endRotationEase_.Update();

	move = endPositionEase_.GetReturn();

	Object3d::SetPosition(move);

	move = {};
	move = endRotationEase_.GetReturn();

	move = {
		move.x,
		move.y,
		move.z };

	Object3d::SetRotation(move);

	Object3d::Update();

	cart_->SetRotation(Object3d::GetRotation());

	if (endPositionEase_.IsEnd()) {
		SetIsOver(true);
	}
}
