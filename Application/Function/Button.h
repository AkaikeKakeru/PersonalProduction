#pragma once

#include "Vector2.h"
#include "Sprite.h"

class Button {
public://メンバ関数
	void Initialize(uint32_t index);
	void Update();
	void Draw();
	void Finalize();

	//マウスクリックを確認
	bool ChackClick(bool whichMouseButtonWasPressed);

public://アクセッサ
	//中心位置取得
	const Vector2& GetPosition() {
		return pos_;
	}
	//サイズ取得
	const Vector2& GetSize() {
		return size_;
	}
	//スプライト取得
	Sprite* GetSprite() {
		return sprite_;
	}

	//中心位置セット
	void SetPosition(const Vector2& position) {
		pos_ = position;
	}
	//サイズセット
	void SetSize(const Vector2& size) {
		size_ = size;
	}
	//スプライトセット
	void SetSprite(Sprite* sprite) {
		sprite_ = sprite;
	}

	//スプライトカラーのセット
	void SetColor(const Vector4& color) {
		sprite_->SetColor(color);
	}

private://メンバ変数
	Vector2 pos_;//中心位置
	Vector2 size_;//サイズ

	Sprite* sprite_;
};