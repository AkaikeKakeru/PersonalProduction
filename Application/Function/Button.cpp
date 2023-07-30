#include "Button.h"
#include "Input.h"
#include "SpriteBasis.h"
#include "SafeDelete.h"

void Button::Initialize(uint32_t index) {
	sprite_ = new Sprite();
	sprite_->SetAnchorPoint({ 0.5f, 0.5f });
	sprite_->Initialize(index);

	pos_ = { 0,0 };
	size_ = { 10,10 };
}

void Button::Update() {
	sprite_->SetPosition(pos_);
	sprite_->SetSize(size_);

	sprite_->Update();
}

void Button::Draw() {
	sprite_->Draw();
}

void Button::Finalize() {
	SafeDelete(sprite_);
}

bool Button::ChackClick(bool whichMouseButtonWasPressed) {
	Vector2 mousePosition = Input::GetInstance()->GetMousePosition();

	if (whichMouseButtonWasPressed) {
		//ボタンと、マウスカーソルが重なっている状態かどうか
		if(mousePosition.x > 
			(pos_.x - (size_.x / 2)) &&
			mousePosition.x < 
			(pos_.x + (size_.x / 2)) &&
			mousePosition.y > 
			(pos_.y - (size_.y / 2)) &&
			mousePosition.y < 
			(pos_.y + (size_.y / 2))) {
			return true;
		}
	}

	return false;
}
