#pragma once
#include "SpriteBasis.h"
#include "Sprite.h"

class Text {
public://メンバ関数
	void Initialize(UINT fontTexNum);

	void Print(const std::string & text, float x, float y, float scale = 1.0f);

	void DrawAll();
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

public:
	Text() = default;
	~Text();
};