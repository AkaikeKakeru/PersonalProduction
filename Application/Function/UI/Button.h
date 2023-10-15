/*スプライトとしてのボタンUI*/

#pragma once

#include "Vector2.h"
#include "Sprite.h"
#include "Text.h"

/*スプライトとしてのボタンUI*/
class Button {
public://メンバ関数
	void Initialize(uint32_t index);
	void Update();
	void Draw();
	void Finalize();

	/// <summary>
	/// マウスクリックを確認
	/// </summary>
	/// <param name="mouseButtonWasPressed">押されたか</param>
	/// <returns></returns>
	bool ChackClick(bool mouseButtonWasClick);

public://アクセッサ
	//中心位置取得
	const Vector2& GetPosition() {
		return position_;
	}
	//サイズ取得
	const Vector2& GetSize() {
		return size_;
	}
	//スプライト取得
	Sprite* GetSprite() {
		return sprite_;
	}
	//テキスト取得
	Text* GetText() {
		return text_;
	}

	//中心位置セット
	void SetPosition(const Vector2& position) {
		position_ = position;

		text_->SetPosition(position);
	}
	//サイズセット
	void SetSize(const Vector2& size) {
		size_ = size;
	}
	//スプライトセット
	void SetSprite(Sprite* sprite) {
		sprite_ = sprite;
	}
	//テキストセット
	void SetText(Text* text) {
		text_ = text;
	}

	//スプライトカラーのセット
	void SetColor(const Vector4& color) {
		sprite_->SetColor(color);
	}

	//テロップのセット
	void SetTelop(const std::string& telop) {
		text_->SetString(telop);
	}

private://メンバ変数
	Vector2 position_;//中心位置
	Vector2 size_;//サイズ

	Sprite* sprite_;//背景スプライト

	Text* text_;//テキスト
	float textSize_ = 0.0f;//テキストのサイズ
};