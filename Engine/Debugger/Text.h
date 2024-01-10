/*テキスト*/

#pragma once
#include "SpriteBasis.h"
#include "Sprite.h"

/*テキスト*/
class Text {
public://メンバ関数
	void Initialize(UINT fontTexNum);

	//テキストをプリント
	void Print();
	//プリントした全ストックを描画
	void DrawAll();

public:
	//中心位置取得
	const Vector2& GetPosition() {
		return position_;
	}
	//サイズ取得
	const Vector2& GetSize() {
		return size_;
	}
	//ストリングの取得
	const std::string& GetString() {
		return string_;
	}

	//中心位置セット
	void SetPosition(const Vector2& position) {
		position_ = {
			position.x - (fontWidth_ * 2.0f * (string_.length() * 1.0f)),
			position.y
		};
	}
	//サイズセット
	void SetSize(const Vector2& size) {
		size_ = size;
	}
	//ストリングのセット
	void SetString(const std::string & text) {
		string_ = text;
	}

public://定数
	static const int maxCharCount_ = 256;
	static const int fontWidth_ = 9;			// フォント画像内1文字分の横幅
	static const int fontHeight_ = 18;		// フォント画像内1文字分の縦幅
	static const int fontLineCount_ = 14;	// フォント画像内1行分の文字数

private://メンバ変数
	static SpriteBasis* spriteBas_;

	//スプライトデータ配列
	Sprite* sprites_[maxCharCount_] = {};
	// 配列の添え字番号
	int spriteIndex_ = 0;

	//位置
	Vector2 position_;
	//スケール
	Vector2 size_;
	//ストリング
	std::string string_;

public:
	Text() = default;
	~Text();
};