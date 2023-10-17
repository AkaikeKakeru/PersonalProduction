/*シーン遷移用オブジェクト*/

#include "FadeTile.h"
#include <SafeDelete.h>

void FadeTile::Initialize(int32_t texIndex) {
	preset_ = preFade_;
	alpha_ = 1.0f;

	sprite_ = new Sprite();
	sprite_->Initialize(texIndex);
	sprite_->SetSize({ WinApp::Win_Width,WinApp::Win_Height });
	sprite_->SetColor({ 0,0,0,1.0f });

	is_ = true;
	isOpen_ = true;
	isEnd_ = false;
}

void FadeTile::Update() {
	if (is_) {
		switch (preset_) {
		case preFade_:
			Fade();
			break;

		default:
			break;
		}
	}

	Vector4 col = sprite_->GetColor();
	col.w = alpha_;

	sprite_->SetColor(col);
	sprite_->Update();
}

void FadeTile::Draw() {
	sprite_->Draw();
}

void FadeTile::Finalize() {
	SafeDelete(sprite_);
}

void FadeTile::Fade() {
	//開くときはアルファ値を減らす。閉めるときは増やす。
	speed_ = isOpen_ ? -kSpeedDefault_ : kSpeedDefault_;

	if (alpha_ > 1.0f) {
		//エンドフラグ
		isEnd_ = true;

		speed_ = 0.0f;
		alpha_ = 1.0f;
	}
	else if (alpha_ < 0.0f) {
		is_ = false;

		speed_ = 0.0f;
		alpha_ = 0.0f;
	}

	alpha_ += speed_;
}
