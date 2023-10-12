#include "Text.h"

SpriteBasis* Text::spriteBas_ = SpriteBasis::GetInstance();

Text::~Text() {
	for (int i = 0; i < _countof(sprites_); i++) {
		delete sprites_[i];
	}
}

void Text::Initialize(UINT fontTexNum) {
	for (int i = 0; i < _countof(sprites_); i++) {
		// スプライト生成
		sprites_[i] = new Sprite();
		sprites_[i]->Initialize(fontTexNum);
	}
}

void Text::Print() {
	// 全ての文字について
	for (size_t i = 0; i < string_.size(); i++) {
		// 最大文字数超過
		if (spriteIndex_ >= maxCharCount_) {
			break;
		}

		// 1文字取り出す(※ASCIIコードでしか成り立たない)
		const unsigned char& character = static_cast<const unsigned char>(string_[i]);

		int fontIndex = character - 32;
		if (character >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount_;
		int fontIndexX = fontIndex % fontLineCount_;

		// 座標計算
		sprites_[spriteIndex_]->SetPosition({ position_.x + fontWidth_ * size_.x * i, position_.y });
		sprites_[spriteIndex_]->SetSize({ fontWidth_ * size_.x, fontHeight_ * size_.y });
		sprites_[spriteIndex_]->SetAnchorPoint({ 0.5f,0.5f });
		sprites_[spriteIndex_]->SetTextureLeftTop({ (float)fontIndexX * fontWidth_, (float)fontIndexY * fontHeight_ });
		sprites_[spriteIndex_]->SetTextureSize({ (float)fontWidth_, (float)fontHeight_ });

		sprites_[spriteIndex_]->Update();

		// 文字を１つ進める
		spriteIndex_++;
	}
}

void Text::DrawAll() {
	// 全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex_; i++) {
		// スプライト描画
		sprites_[i]->Draw();
	}

	spriteIndex_ = 0;
}
