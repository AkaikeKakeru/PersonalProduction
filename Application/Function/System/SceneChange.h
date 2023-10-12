﻿#pragma once
#include "Sprite.h"

class SceneChange {
public:
	//プリセット名
	enum PresetName {
		preFade_ //フェード
	};

public: //定数
	//デフォルト速さ
	const float kSpeedDefault_ = 0.1f;

public: //メンバ関数
	void Initialize(int32_t texIndex);
	void Update();
	void Draw();
	void Finalize();

	//フェード用
	void Fade();

public: //アクセッサ
	//プリセットのセット
	void SetPreset(int presetNum) {
		preset_ = presetNum;
	}

	//色のセット
	void SetColor(const Vector4& color) {
		sprite_->SetColor(color);
	}

	//サイズのセット
	void SetSize(const Vector2& size) {
		sprite_->SetSize(size);
	}

	//稼働フラグの取得
	bool Is() {
		return is_;
	}
	//オープンフラグの取得
	bool IsOpen() {
		return isOpen_;
	}
	//終了フラグの取得
	bool IsEnd() {
		return isEnd_;
	}

	//稼働フラグのセット
	void SetIs(bool is) {
		is_ = is;
	}

	//オープンフラグのセット
	void SetIsOpen(bool is) {
		isOpen_ = is;
	}

	//終了フラグのセット
	void SetIsEnd(bool is) {
		isEnd_ = is;
	}

private:
	//プリセット
	int preset_ = preFade_;
	//スプライト
	Sprite* sprite_;
	//アルファ値
	float alpha_ = 1.0f;
	//速さ
	float speed_ = kSpeedDefault_;

	//稼働フラグ
	bool is_ = false;
	//オープンフラグ
	bool isOpen_ = false;
	//終了フラグ
	bool isEnd_ = false;
};