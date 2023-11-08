/*調整項目をひとまとめにしたい*/
#pragma once

//グローバル変数
class AdjustmentVariables {
private: //シングルトン化
	AdjustmentVariables() = default;
	~AdjustmentVariables() = default;
	//コピーコンストラクタを無効
	AdjustmentVariables(const AdjustmentVariables&) = delete;
	//代入演算子を無効
	const AdjustmentVariables& operator=(const AdjustmentVariables&) = delete;
public: //static
	static AdjustmentVariables* GetInstance();
};