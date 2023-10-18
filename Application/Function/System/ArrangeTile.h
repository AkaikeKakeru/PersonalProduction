/*タイルを並べるようなシーン遷移オブジェクト*/

#pragma once
#include "Sprite.h"
#include <vector>

/*タイルを並べるようなシーン遷移オブジェクト*/
class ArrangeTile {
public: //定数
	//デフォルト速さ
	const float cSpeedDefault_ = 0.1f;

public: //メンバ関数
	void Initialize(
		int32_t texIndex,
		const Vector2 pos,
		const float rotation,
		const Vector2 size,
		const int value);
	void Update();
	void Draw();
	void Finalize();

	//スプライト追加
	void AddSprite(
		int32_t texIndex,
		const Vector2 pos,
		const float rotation,
		const Vector2 size);

	//リセット
	void Reset(	
		bool is,
		bool isOpen);

private:
	//ベクター3をベクター2に変換
	Vector2 ConvertVector3ToVector2(const Vector3 v);
	//ベクター2をベクター3に変換
	Vector3 ConvertVector2ToVector3(const Vector2 v);

public: //アクセッサ
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
	//スプライト
	std::vector<std::unique_ptr<Sprite>> sprites_;

	//通過位置
	Vector2 posPass_;

	//開始位置
	Vector2 posStart_;
	//終了位置
	Vector2 posEnd_;

	//開始サイズ
	Vector2 sizeStart_;
	//終了サイズ
	Vector2 sizeEnd_;

	//タイマー最大値
	int32_t timerMax_ = 0;
	//タイマー現在値
	int32_t timerNow_ = 0;

	//稼働フラグ
	bool is_ = false;
	//オープンフラグ
	bool isOpen_ = false;
	//終了フラグ
	bool isEnd_ = false;

	//基準サイズ
	Vector2 judgeSize_;
};