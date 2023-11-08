/*調整項目をひとまとめにしたい*/
#pragma once
#include <Vector3.h>
#include <variant>
#include <map>
#include <string>

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
		std::map<std::string, Item> items_;
	};

public: //関数
	/// <summary>
	/// グループ作成
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);

private: //変数
	//全データ
	std::map<std::string, Group> datas_;

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