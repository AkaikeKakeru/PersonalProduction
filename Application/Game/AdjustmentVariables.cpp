#include "AdjustmentVariables.h"

void AdjustmentVariables::CreateGroup(const std::string& groupName) {
	//指定名のオブジェクトが無ければ追加
	datas_[groupName];
}

void AdjustmentVariables::SetValue(
	const std::string& groupName,
	const std::string& key,
	int32_t value) {
	//グループの参照を取得
	Group& group = datas_[groupName];

	//新しい項目のデータを設定
	Item newItem{};
	newItem.value_ = value;

	//設定した項目をstd::mapに追加
	group.items_[key] = newItem;
}

void AdjustmentVariables::SetValue(
	const std::string& groupName, 
	const std::string& key,
	float value) {
	//グループの参照を取得
	Group& group = datas_[groupName];

	//新しい項目のデータを設定
	Item newItem{};
	newItem.value_ = value;

	//設定した項目をstd::mapに追加
	group.items_[key] = newItem;
}

void AdjustmentVariables::SetValue(
	const std::string& groupName,
	const std::string& key,
	const Vector3& value) {
	//グループの参照を取得
	Group& group = datas_[groupName];

	//新しい項目のデータを設定
	Item newItem{};
	newItem.value_ = value;

	//設定した項目をstd::mapに追加
	group.items_[key] = newItem;
}

AdjustmentVariables* AdjustmentVariables::GetInstance() {
	static AdjustmentVariables instance;
	return &instance;
}
