#include "Button.h"

#include "Input.h"

#include "DrawBasis.h"

void Button::Initialize(uint32_t index) {
	sprite_ = new Sprite();
	sprite_->SetAnchorPoint({ 0.5f, 0.5f });
	sprite_->Initialize(DrawBasis::GetInstance(), index);

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

bool Button::ChackClick(bool whichMouseButtonWasPressed) {
	Vector2 mousePosition = Input::GetInstance()->GetMousePosition();

	if (whichMouseButtonWasPressed) {
		//�{�^���ƁA�}�E�X�J�[�\�����d�Ȃ��Ă����Ԃ��ǂ���
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
