#include "AdjustmentVariables.h"
#include <imgui.h>

void AdjustmentVariables::CreateGroup(const std::string& groupName) {
	//指定名のオブジェクトが無ければ追加
	datas_[groupName];
}

void AdjustmentVariables::Update() {
	if (!ImGui::Begin("Adjustment Variables", nullptr, ImGuiWindowFlags_MenuBar)) return;
	if (!ImGui::BeginMenuBar()) return;

	for (std::map<std::string,Group>::iterator itGroup = datas_.begin();
		itGroup != datas_.end();
		++itGroup) {
		//グループ名を取得
		const std::string& groupName = itGroup->first;
		//グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) continue;

		//各項目について
		for (std::map<std::string, Item>::iterator itItem = group.items_.begin();
			itItem != group.items_.end();
			++itItem) {
			//項目名を取得
			const std::string& iteName = itItem->first;

			//項目の参照を取得
			Item& item = itItem->second;

			/*保持している値の型で分岐*/

			//int32_t
			if (std::holds_alternative<int32_t>(item.value_)) {
				int32_t* ptr = std::get_if<int32_t>(&item.value_);
				ImGui::SliderInt(
					iteName.c_str(), 
					ptr,
					0, 100);
			}

			//float
			else if (std::holds_alternative<float>(item.value_)) {
				float* ptr = std::get_if<float>(&item.value_);
				ImGui::SliderFloat(
					iteName.c_str(), 
					ptr,
					0, 100);
			}

			//Vector3
			else if (std::holds_alternative<Vector3>(item.value_)) {
				Vector3* ptr = std::get_if<Vector3>(&item.value_);
				ImGui::SliderFloat3(
					iteName.c_str(),
					reinterpret_cast<float*>(ptr),
					-10.0f,10.0f);
			}
		}
		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
}

void AdjustmentVariables::SaveFile(const std::string& groupName) {
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	//未登録チェック
	assert(itGroup != datas_.end());

	json root;
	root = json::object();

	//jsonオブジェクト登録
	root[groupName] = json::object();

	//各項目について
	//各項目について
	for (std::map<std::string, Item>::iterator itItem = itGroup->second.items_.begin();
		itItem != itGroup->second.items_.end();
		++itItem) {
		//項目名を取得
		const std::string& iteName = itItem->first;

		//項目の参照を取得
		Item& item = itItem->second;

		/*保持している値の型で分岐*/

		//int32_t
		if (std::holds_alternative<int32_t>(item.value_)) {
			root[groupName][iteName] = std::get<int32_t>(item.value_);
		}

		//float
		else if (std::holds_alternative<float>(item.value_)) {
			root[groupName][iteName] = std::get<float>(item.value_);
		}

		//Vector3
		else if (std::holds_alternative<Vector3>(item.value_)) {
			Vector3 value = std::get<Vector3>(item.value_);
			root[groupName][iteName] = json::array({ value.x,value.y,value.z });
		}
	}
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
