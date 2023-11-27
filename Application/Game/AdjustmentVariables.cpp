#include "AdjustmentVariables.h"
#include <imgui.h>
#include <fstream>
#include <Windows.h>

void AdjustmentVariables::CreateGroup(const std::string& groupName) {
	//指定名のオブジェクトが無ければ追加
	datas_[groupName];
}

void AdjustmentVariables::Update() {
	if (!ImGui::Begin("Adjustment Variables", nullptr, ImGuiWindowFlags_MenuBar)) return;
	if (!ImGui::BeginMenuBar()) return;

	for (std::map<std::string, Group>::iterator itGroup = datas_.begin();
		itGroup != datas_.end();
		++itGroup) {
		//グループ名を取得
		const std::string& groupName = itGroup->first;
		//グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) continue;

#pragma region 項目内の値の表示
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
					-10.0f, 10.0f);
			}
		}
#pragma endregion
		//改行
		ImGui::Text("\n");
#pragma region セーブ
		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved", groupName);
			MessageBoxA(nullptr, message.c_str(), "adjustmentVariable", 0);
		}
#pragma endregion
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

#pragma region 保存
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
#pragma endregion

#pragma region 書き出し
	//ディレクトリパスのローカル変数
	std::filesystem::path dir(kDirectoryPath_);
	//ディレクトリがなければ作成
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	//書き込むJsonファイルのフルパスを合成
	std::string filePath = kDirectoryPath_ + groupName + ".json";

	//書き込み用ファイルストリーム
	std::ofstream ofs;
	ofs.open(filePath);

	//ファイルのオープンに失敗
	if (ofs.fail()) {
		std::string message = "Failed open data file write.";
		MessageBoxA(nullptr, message.c_str(), "adjustmentVariable", 0);
		assert(0);
		return;
	}

	//ファイルにJson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();
#pragma endregion
}

void AdjustmentVariables::LoadFiles() {
	//ディレクトリパスのローカル変数
	std::filesystem::path dir(kDirectoryPath_);
	//ディレクトリがなければスキップ
	if (!std::filesystem::exists(dir)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(dir);
	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();

		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		//ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

void AdjustmentVariables::LoadFile(const std::string& groupName) {
	//読み込むJsonファイルのフルパスを合成
	std::string filePath = kDirectoryPath_ + groupName + ".json";

	//読み込み用ファイルストリーム
	std::ifstream ifs;
	ifs.open(filePath);

	//ファイルのオープンに失敗
	if (ifs.fail()) {
		std::string message = "Failed open data file load.";
		MessageBoxA(nullptr, message.c_str(), "adjustmentVariable", 0);
		assert(0);
		return;
	}

	json root;
	//Json文字列からJsonデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//グループを検索
	json::iterator itGroup = root.find(groupName);

	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (json::iterator itItem = itGroup->begin();
		itItem != itGroup->end();
		++itItem) {
		//アイテム名を取得
		const std::string& iteName = itItem.key();

		/*保持している値の型で分岐*/

		//int32_t
		if (itItem->is_number_integer()) {
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, iteName, value);
		}

		//float
		else if (itItem->is_number_float()) {
			//double型で取得し、float型に変換
			double value = itItem->get<double>();
			SetValue(groupName, iteName, static_cast<float>(value));
		}

		//要素数3つの配列 (Vector3)
		else if (itItem->is_array()
			&& itItem->size() == 3) {
			Vector3 value = {
				itItem->at(0),
				itItem->at(1),
				itItem->at(2)
			};
			SetValue(groupName, iteName, value);
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

void AdjustmentVariables::AddItem(const std::string& groupName, const std::string& key, int32_t value) {
	//グループの参照を取得
	Group& group = datas_[groupName];

	//項目が未登録なら
	if (group.items_.count(key) == 0) {
		SetValue(groupName, key, value);
	}
}

void AdjustmentVariables::AddItem(const std::string& groupName, const std::string& key, float value) {
	//グループの参照を取得
	Group& group = datas_[groupName];

	//項目が未登録なら
	if (group.items_.count(key) == 0) {
		SetValue(groupName, key, value);
	}
}

void AdjustmentVariables::AddItem(const std::string& groupName, const std::string& key, const Vector3& value) {
	//グループの参照を取得
	Group& group = datas_[groupName];

	//項目が未登録なら
	if (group.items_.count(key) == 0) {
		SetValue(groupName, key, value);
	}
}

int32_t AdjustmentVariables::GetIntValue(
	const std::string& groupName,
	const std::string& key) const {

	//未登録チェック
	assert(datas_.count(groupName));

	const Group& group = datas_.at(groupName);

	assert(group.items_.count(key));

	return std::get<int32_t>(group.items_.at(key).value_);
}

float AdjustmentVariables::GetFloatValue(
	const std::string& groupName,
	const std::string& key) const {

	//未登録チェック
	assert(datas_.count(groupName));

	const Group& group = datas_.at(groupName);

	assert(group.items_.count(key));

	return	std::get<float>(group.items_.at(key).value_);
}

Vector3 AdjustmentVariables::GetVector3Value(
	const std::string& groupName,
	const std::string& key) const {

	//未登録チェック
	assert(datas_.count(groupName));

	const Group& group = datas_.at(groupName);

	assert(group.items_.count(key));

	return	std::get<Vector3>(group.items_.at(key).value_);
}

AdjustmentVariables* AdjustmentVariables::GetInstance() {
	static AdjustmentVariables instance;
	return &instance;
}
