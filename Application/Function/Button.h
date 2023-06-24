#pragma once

#include "Vector2.h"
#include "Sprite.h"

class Button {
public://�����o�֐�
	void Initialize(uint32_t index);
	void Update();
	void Draw();

	//�}�E�X�N���b�N���m�F
	bool ChackClick(bool whichMouseButtonWasPressed);

public://�A�N�Z�b�T
	//���S�ʒu�擾
	const Vector2& GetPosition() {
		return pos_;
	}
	//�T�C�Y�擾
	const Vector2& GetSize() {
		return size_;
	}
	//�X�v���C�g�擾
	Sprite* GetSprite() {
		return sprite_;
	}

	//���S�ʒu�Z�b�g
	void SetPosition(const Vector2& position) {
		pos_ = position;
	}
	//�T�C�Y�Z�b�g
	void SetSize(const Vector2& size) {
		size_ = size;
	}
	//�X�v���C�g�Z�b�g
	void SetSprite(Sprite* sprite) {
		sprite_ = sprite;
	}

private://�����o�ϐ�
	Vector2 pos_;//���S�ʒu
	Vector2 size_;//�T�C�Y

	Sprite* sprite_;
};