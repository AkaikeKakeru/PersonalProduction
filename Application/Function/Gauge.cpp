#include "Gauge.h"
#include "SafeDelete.h"

void Gauge::Initialize() {
	spriteRest_ = new Sprite();
	spriteRest_->Initialize(2);
	spriteRest_->SetTextureSize({ 64, 64 });
	spriteRest_->SetAnchorPoint({ 0.5f,0.5f });
	spriteRest_->SetTextureLeftTop({ 64,64 });
	spriteRest_->SetColor({ 0.2f,0.7f,0.2f,1.0f });

	spriteGauge_ = new Sprite();
	spriteGauge_->Initialize(2);
	spriteGauge_->SetTextureSize({ 32, 64 });
	spriteGauge_->SetTextureLeftTop({ 16,64 });
	spriteGauge_->SetAnchorPoint({ 0.5f,0.5f });

	spriteLeft_ = new Sprite();
	spriteLeft_->Initialize(2);
	spriteLeft_->SetTextureSize({ 64, 64 });
	spriteLeft_->SetTextureLeftTop({ 0,0 });
	spriteLeft_->SetAnchorPoint({ 0.5f,0.5f });

	spriteRight_ = new Sprite();
	spriteRight_->Initialize(2);
	spriteRight_->SetTextureSize({ 64, 64 });
	spriteRight_->SetTextureLeftTop({ 64,0 });
	spriteRight_->SetAnchorPoint({ 0.5f,0.5f });

	spriteRest_->SetSize({ size_.x * 5,size_.y });
	spriteGauge_->SetSize({ size_.x * 4,size_.y });
	spriteLeft_->SetSize({ size_.x,size_.y});
	spriteRight_->SetSize({ size_.x,size_.y});

	spriteRest_->SetPosition({
		positionGauge_.x + (lengthGauge_ * (lengthGauge_ / 2)) +
		positionGaugeOffset_.x,
		positionGauge_.y +
		positionGaugeOffset_.y });

	spriteGauge_->SetPosition({
		positionGauge_.x + (lengthGauge_ * (lengthGauge_ / 2)) +
		positionGaugeOffset_.x,
		positionGauge_.y +
		positionGaugeOffset_.y });

	spriteLeft_->
		SetPosition({
		positionGauge_.x +
		positionGaugeOffset_.x,
			positionGauge_.y +
			positionGaugeOffset_.y });

	spriteRight_->
		SetPosition({
		positionGauge_.x + (lengthGauge_ * ((lengthGauge_ / 4) * (lengthGauge_ / 4))) +
		positionGaugeOffset_.x,
			positionGauge_.y +
			positionGaugeOffset_.y });
}

void Gauge::Update() {
	//フラグ立ってるならゲージを変動させる
	if (isFluct_) {
		Fluctuation();

		//終了時間に到達したら、タイマーリセット
		if (nowTime_ >= maxTime_) {
			nowTime_ = 0;
			isFluct_ = false;
		}
		else {
			nowTime_++;
		}
	}

	spriteGauge_->Update();
	spriteLeft_->Update();
	spriteRight_->Update();
	spriteRest_->Update();
}

void Gauge::Draw() {
	spriteRest_->Draw();
	spriteGauge_->Draw();
	spriteLeft_->Draw();
	spriteRight_->Draw();
}

void Gauge::Finalize() {
	SafeDelete(spriteLeft_);
	SafeDelete(spriteRight_);
	SafeDelete(spriteGauge_);
	SafeDelete(spriteRest_);
}

void Gauge::Fluctuation() {
	//サイズの変動
	Vector3 reSize = {};
	//座標の変動
	Vector3 move = {};

	reSize = EaseInOut(
		startEaseSize_,
		endEaseSize_,
		nowTime_ / maxTime_);

	move = EaseInOut(
		startEasePosition_,
		endEasePosition_,
		nowTime_ / maxTime_);

	spriteRest_->SetSize({ reSize.x,reSize.y });
	spriteRest_->SetPosition({ move.x,move.y });
}

void Gauge::DecisionFluctuation() {
	//残量が0以下なら
	if (rest_ <= 0) {
		startEaseSize_ = {
			spriteRest_->GetSize().x,
			spriteRest_->GetSize().y,
			0
		};
		endEaseSize_ = {
			0,
			lengthGauge_ * 4 ,
			0
		};

		startEasePosition_ = {
			spriteRest_->GetPosition().x,
			spriteRest_->GetPosition().y,
			0
		};
		endEasePosition_ = {
			positionGauge_.x +
			0 +
			positionGaugeOffset_.x,
			positionGauge_.y +
			positionGaugeOffset_.y,
			0
		};
	}
	else {
		startEaseSize_ = {
			spriteRest_->GetSize().x,
			spriteRest_->GetSize().y,
			0
		};
		endEaseSize_ = {
			(lengthGauge_ * 4 / 2) * rest_,
			lengthGauge_ * 4 ,
			0
		};

		startEasePosition_ = {
			spriteRest_->GetPosition().x,
			spriteRest_->GetPosition().y,
			0
		};
		endEasePosition_ = {
			positionGauge_.x +
			(lengthGauge_ * (rest_ - 2.0f)) +
			positionGaugeOffset_.x,
			positionGauge_.y +
			positionGaugeOffset_.y,
			0
		};
	}

	isFluct_ = true;
}

void Gauge::ResetSize() {
	spriteRest_->SetSize({ size_.x * 5,size_.y });
	spriteGauge_->SetSize({ size_.x * 4,size_.y });
	spriteLeft_->SetSize({ size_.x,size_.y});
	spriteRight_->SetSize({ size_.x,size_.y});
}

void Gauge::ResetPosition() {
	spriteRest_->SetPosition({
		positionGauge_.x + (lengthGauge_ * (lengthGauge_ / 2)) +
		positionGaugeOffset_.x,
		positionGauge_.y +
		positionGaugeOffset_.y });

	spriteGauge_->SetPosition({
		positionGauge_.x + (lengthGauge_ * (lengthGauge_ / 2)) +
		positionGaugeOffset_.x,
		positionGauge_.y +
		positionGaugeOffset_.y });

	spriteLeft_->
		SetPosition({
		positionGauge_.x +
		positionGaugeOffset_.x,
			positionGauge_.y +
			positionGaugeOffset_.y });

	spriteRight_->
		SetPosition({
		positionGauge_.x + (lengthGauge_ * ((lengthGauge_ / 4) * (lengthGauge_ / 4))) +
		positionGaugeOffset_.x,
			positionGauge_.y +
			positionGaugeOffset_.y });
}
