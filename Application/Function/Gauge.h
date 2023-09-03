#pragma once
#include "Sprite.h"
#include "Vector3.h"

class Gauge {
public: //メンバ関数
	void Initialize();
	void Update ();
	void Draw();
	void Finalize();

	//ゲージ変動
	void Fluctuation();

	//変動量決め
	void DecisionFluctuation();

	//サイズの再設定
	void ResetSize();

	//位置の再設定
	void ResetPosition();

public: //アクセッサ
	//サイズの取得
	const Vector2& GetSize() const {
		return size_;
	};
	//サイズのセット
	void SetSize(Vector2 const size) {
		size_ = size;

		ResetSize();
	};

	//位置(左上角)の取得
	const Vector2& GetPosition() const {
		return positionGauge_;
	};
	//位置(左上角)のセット
	void SetPosition(Vector2 const position) {
		positionGauge_ = position;

		ResetPosition();
	};

	//位置オフセットのセット
	void SetPositionOffset(Vector2 const offset) {
		positionGaugeOffset_ = offset;
	};

	//残量スプライトの取得
	Sprite* GetRestSprite() {
		return spriteRest_;
	};
	//左スプライトの取得
	Sprite* GetLeftSprite() {
		return spriteLeft_;
	};
	//右スプライトの取得
	Sprite* GetRightSprite() {
		return spriteRight_;
	};
	//ゲージスプライトの取得
	Sprite* GetGaugeSprite() {
		return spriteGauge_;
	};

	//変動フラグの取得
	bool IsFluct() {
		return isFluct_;
	};

	//変動フラグのセット
	void SetIsFluct(bool isFluct) {
		isFluct_ = isFluct;
	}

	//残量の取得
	float GetRest() {
		return rest_;
	}

	//残量のセット
	void SetRest(float rest) {
		rest_ = rest;
	}

	//長さの取得
	float GetLength() {
		return lengthGauge_;
	}

	//長さのセット
	void SetLength(float length) {
		lengthGauge_ = length;
	}

	//最大時間の取得
	float GetMaxTime() {
		return maxTime_;
	}

	//最大時間のセット
	void SetMaxTime(float maxTime) {
		maxTime_ = maxTime;
	}

private: //メンバ変数
	//残量
	float rest_ = 0.0f;

	//ゲージ1メモリ当たりの長さ
	float lengthGauge_ = 16.0f;

	//サイズ
	Vector2 size_ = {
		64.0f,
		64.0f
	};

	//ゲージの位置(左上角)
	Vector2 positionGauge_ = {
		lengthGauge_ * 2,
		lengthGauge_ * 2 
	};

	//ゲージ位置のオフセット
	Vector2 positionGaugeOffset_ = {
		lengthGauge_ / 2,
		lengthGauge_ / 2 
	};

	//ゲージ左端用スプライト
	Sprite* spriteLeft_ = nullptr;
	//ゲージ右端用スプライト
	Sprite* spriteRight_ = nullptr;
	//ゲージ用スプライト
	Sprite* spriteGauge_ = nullptr;
	//残量用スプライト
	Sprite* spriteRest_ = nullptr;

	//位置イージング始発点
	Vector3 startEasePosition_{};
	//位置イージング終着点
	Vector3 endEasePosition_{};

	//サイズイージング始発点
	Vector3 startEaseSize_{};
	//サイズイージング終着点
	Vector3 endEaseSize_{};

	//イージング最大時間
	float maxTime_ = 30.0f;
	//イージング経過時間
	float nowTime_ = 0;

	//イージング実行フラグ
	bool isFluct_ = false;
};