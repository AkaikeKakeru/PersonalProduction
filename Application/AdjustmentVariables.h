/*調整項目をひとまとめにしたい*/
#pragma once
#include <Vector3.h>
#include <variant>
#include <map>

//調整項目変数のまとめ
class AdjustmentVariables {
public: //構造
	//項目 の構造
	struct Item {
		//値
		std::variant<int32_t, float, Vector3> value_;
	};

	//グループの構造
	struct Group {
		//項目
		std::map<std::strong_ordering, Item> items_;
	};

private: //変数
	//全データ
	std::map<std::strong_ordering, Group> datas_;

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