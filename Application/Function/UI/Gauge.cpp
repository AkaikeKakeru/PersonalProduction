/*スプライト型のゲージUI*/

#include "Gauge.h"
#include "SafeDelete.h"
#include <Framework.h>

void Gauge::Initialize() {
	float texSize = 64.0f;

	spriteRest_ = new Sprite();
	spriteRest_->Initialize(Framework::kGaugeTextureIndex_);
	spriteRest_->SetTextureSize({ texSize, texSize });
	spriteRest_->SetTextureLeftTop({ 64,64 });
	spriteRest_->SetColor({ 0.2f,0.7f,0.2f,1.0f });

	spriteGauge_ = new Sprite();
	spriteGauge_->Initialize(Framework::kGaugeTextureIndex_);
	spriteGauge_->SetTextureSize({ texSize / 2, texSize });
	spriteGauge_->SetTextureLeftTop({ 16,64 });

	spriteLeft_ = new Sprite();
	spriteLeft_->Initialize(Framework::kGaugeTextureIndex_);
	spriteLeft_->SetTextureSize({ texSize, texSize });
	spriteLeft_->SetTextureLeftTop({ 0,0 });

	spriteRight_ = new Sprite();
	spriteRight_->Initialize(Framework::kGaugeTextureIndex_);
	spriteRight_->SetTextureSize({ texSize, texSize });
	spriteRight_->SetTextureLeftTop({ 64,0 });

	spriteRest_->SetAnchorPoint({ 0.0f,0.5f });
	spriteGauge_->SetAnchorPoint({ 0.5f,0.5f });
	spriteLeft_->SetAnchorPoint({ 0.5f,0.5f });
	spriteRight_->SetAnchorPoint({ 0.5f,0.5f });

	ResetSize();
	ResetArrangement();
}

void Gauge::Update() {
	//フラグ立ってるならゲージを変動させる
	if (isFluct_) {
		Fluctuation();

		//終了時間に到達したら、タイマーリセット
		if (nowTime_ >= maxTime_) {
			isFluct_ = false;
		}
		else {
			nowTime_++;
		}
	}
	else {
		nowTime_ = 0;
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
			(kDefaultTexSize * sizeGauge_.y),
			0
		};

		startEasePosition_ = {
			spriteRest_->GetPosition().x - sizeGauge_.x,
			spriteRest_->GetPosition().y,
			0
		};

		endEasePosition_ = {
			positionGauge_.x - ((kDefaultTexSize / 2) * sizeGauge_.x),
			positionGauge_.y,
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
			(kDefaultTexSize * sizeGauge_.x * 5) * (rest_ / restMax_),
			(kDefaultTexSize * sizeGauge_.y),
			0
		};

		startEasePosition_ = {
			spriteRest_->GetPosition().x,
			spriteRest_->GetPosition().y,
			0
		};

		endEasePosition_ = {
			positionGauge_.x - ((kDefaultTexSize / 2) * sizeGauge_.x),
			positionGauge_.y,
			0
		};
	}
}

void Gauge::ResetSize() {
	spriteRest_->SetSize({
		(kDefaultTexSize * sizeGauge_.x * 5) * (rest_ / restMax_),
		kDefaultTexSize * sizeGauge_.y
		});
	spriteRest_->SetColor({
		0.2f,0.7f,0.2f,1.0f
		});
	spriteGauge_->SetSize({
		kDefaultTexSize * sizeGauge_.x * 4,
		kDefaultTexSize * sizeGauge_.y
		});
	spriteLeft_->SetSize({
		kDefaultTexSize * sizeGauge_.x,
		kDefaultTexSize * sizeGauge_.y
		});
	spriteRight_->SetSize({
		kDefaultTexSize * sizeGauge_.x,
		kDefaultTexSize * sizeGauge_.y
		});
}

void Gauge::ResetArrangement() {
	//アンカーが他とは違う上で、ゲージ全体の左上に位置を合わせる
	spriteRest_->SetPosition({
		positionGauge_.x - ((kDefaultTexSize / 2) * sizeGauge_.x),
		positionGauge_.y
		});

	//ゲージ中腹は、左端から2枚目の位置が、左上位置とする
	spriteGauge_->SetPosition({
		positionGauge_.x + (kDefaultTexSize * sizeGauge_.x * 2),
		positionGauge_.y
		});

	//ゲージ左端はゲージの左上位置に合わせる
	spriteLeft_->SetPosition({
		positionGauge_.x ,
		positionGauge_.y
		});

	//ゲージの右端は一番右に配置されるように合わせる
	spriteRight_->SetPosition({
		positionGauge_.x + (kDefaultTexSize * sizeGauge_.x * 4),
		positionGauge_.y
		});
}
