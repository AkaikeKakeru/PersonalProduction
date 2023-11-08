/*シェイク*/

#pragma once
#include "Vector3.h"

/*シェイク*/
class Shake {
public:
	void Update();

	/// <summary>
	/// 位置のシェイク
	/// </summary>
	/// <param name="widthSwing">振れ幅</param>
	void ShakePosition(float widthSwing);

public:
	//出力取得
	Vector3 GetOutput() {
		return output_;
	}

	//振れ幅取得
	float GetWidthSwing() {
		return widthSwing_;
	}

	//フラグ取得
	bool Is() {
		return is_;
	}

	//出力のセット
	void SetOutput(Vector3& output) {
		output_ = output;
	}

	//振れ幅のセット
	void SetWidthSwing(float widthSwing) {
		widthSwing_ = widthSwing;
	}

	//フラグのセット
	void SetIs(bool is) {
		is_ = is;
	}

private:
	//出力
	Vector3 output_ = { 0,0,0 };

	//振れ幅
	float widthSwing_ = 0;

	//フラグ
	bool is_ = false;

};