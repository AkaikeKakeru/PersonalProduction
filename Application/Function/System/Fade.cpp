/*シーン遷移用オブジェクト*/

#include "Fade.h"
#include <SafeDelete.h>

void Fade::Initialize(int32_t texIndex) {
	alpha_ = 1.0f;

	sprite_ = new Sprite();
	sprite_->Initialize(texIndex);
	sprite_->SetSize({ WinApp::Win_Width,WinApp::Win_Height });
	sprite_->SetColor({ 0,0,0,1.0f });

	is_ = true;
	isOpen_ = true;
	isEnd_ = false;
}

void Fade::Update() {
	if (is_) {
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

	Vector4 col = sprite_->GetColor();
	col.w = alpha_;

	sprite_->SetColor(col);
	sprite_->Update();
}

void Fade::Draw() {
	sprite_->Draw();
}

void Fade::Finalize() {
	SafeDelete(sprite_);
}
