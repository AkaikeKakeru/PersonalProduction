/*調整項目をひとまとめにしたい*/
#pragma once
#include <Vector3.h>
#include <variant>
#include <map>
#include <string>
#include <json.hpp>

//調整項目変数のまとめ
class AdjustmentVariables {
private: //省略
	using json = nlohmann::json;

public: //定数
	//調整項目変数のパス
	const std::string kDirectoryPath_ = "Resource/adjustmentVariable/";

public: //構造
	//項目の構造
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

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ファイルに保存(書き出す)
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void SaveFile(const std::string& groupName);

	/// <summary>
	/// ディレクトリ内の全ファイル読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイルから読み込み
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void LoadFile(const std::string& groupName);

private: //固有の関数
	//値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);

	//値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);

	//値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

public: //アクセッサ
	//項目の追加(int)
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);

	//項目の追加(float)
	void AddItem(const std::string& groupName, const std::string& key, float value);

	//項目の追加(Vector3)
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);

	//値の取得(int)
	int32_t GetIntValue(const std::string& groupName, const std::string& key) const;
	//値の取得(float)
	float GetFloatValue(const std::string& groupName, const std::string& key) const;
	//値の取得(Vector3)
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) const;

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