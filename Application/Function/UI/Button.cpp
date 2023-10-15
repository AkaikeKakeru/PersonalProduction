/*スプライトとしてのボタンUI*/

#include "Button.h"
#include "Input.h"
#include "SpriteBasis.h"
#include "SafeDelete.h"
#include <Framework.h>

void Button::Initialize(uint32_t index) {
	position_ = { 0,0 };
	size_ = { 10,10 };

	sprite_ = new Sprite();
	sprite_->SetAnchorPoint({ 0.5f, 0.5f });
	sprite_->Initialize(index);

	textSize_ = 2.5f;

	text_ = new Text();
	text_->Initialize(Framework::kTextTextureIndex_);
	text_->SetString("Personal Production");
	text_->SetPosition(position_);
	text_->SetSize({ textSize_,textSize_ });

}

void Button::Update() {
	sprite_->SetPosition(position_);
	sprite_->SetSize(size_);

	sprite_->Update();

	text_->Print();
}

void Button::Draw() {
	sprite_->Draw();

	text_->DrawAll();
}

void Button::Finalize() {
	SafeDelete(sprite_);
	SafeDelete(text_);
}

bool Button::ChackClick(bool mouseButtonWasClick) {
	Vector2 mousePosition = Input::GetInstance()->GetMousePosition();

	if (mouseButtonWasClick) {
		//ボタンと、マウスカーソルが重なっている状態かどうか
		if(mousePosition.x > 
			(position_.x - (size_.x / 2)) &&
			mousePosition.x < 
			(position_.x + (size_.x / 2)) &&
			mousePosition.y > 
			(position_.y - (size_.y / 2)) &&
			mousePosition.y < 
			(position_.y + (size_.y / 2))) {
			return true;
		}
	}

	return false;
}
