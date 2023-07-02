#pragma once
#include "Vector3.h"

class DirectionalLight {
public://サブクラス
	struct ConstBufferData{
		Vector3 lightv_; //ライトへの方向ベクトル
		float pad1_;
		Vector3 lightcolor_;//ライトの色
		unsigned int active_;//有効フラグ
	};

	//ライト方向を取得
	const Vector3& GetLightDir() {
		return lightdir_;
	}

	//ライト色を取得
	const Vector3&  GetLightColor() {
		return lightcolor_;
	}

	//ライト方向をセット
	void SetLightDir(const Vector3& lightdir) {
		//正規化してセット
		lightdir_ = Vector3Normalize(lightdir);
	}

	//ライト色をセット
	void SetLightColor(const Vector3& lightcolor) {
		lightcolor_ = lightcolor;
	}

	//有効フラグをセット
	inline void SetActive(bool active) { active_ = active; }

	//有効チェック
	inline bool IsActive() { return active_; }

private://メンバ変数
	//ライト光線方向(単位ベクトル)
	Vector3 lightdir_ = { 1,0,0 };
	//ライト色
	Vector3 lightcolor_ = { 1,1,1 };
	//有効フラグ
	bool active_ = false;
};