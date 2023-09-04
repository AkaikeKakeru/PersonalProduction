#include "LevelLoader.h"
#include <json.hpp>
#include <cassert>
#include <fstream>
#include <map>
#include <MyMath.h>
#include <SafeDelete.h>

const std::string LevelLoader::kDefaultBaseDirectory_ = "Resource/levels/";

void LevelLoader::LoadFileJson(const std::string fileName,Camera* camera) { 
	////連結してフルパスを得る
	//const std::string fullpath =
	//	std::string("Resource/levels/") + "rimuta_TL1" + ".json";

	//連結してフルパスを得る
	const std::string fullpath =
		kDefaultBaseDirectory_ + fileName + ".json";

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullpath);
	//ファイルオープンの失敗を確認
	assert(!file.fail());

	//解凍したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name =
		deserialized["name"].get<std::string>();
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンスを生成
	LevelData* levelData = new LevelData();

	//走査
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		//種別を取得
		std::string type = object["type"].get<std::string>();

		//メッシュ
		if (type.compare("MESH") == 0) {
			//要素追加
			levelData->objects_.emplace_back(LevelData::ObjectData{});
			//今追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData->objects_.back();

			if (object.contains("file_name")) {
				//ファイル名
				objectData.fileName_ = object["file_name"];
			}

			//トランスフォームのパラメータ読込
			nlohmann::json& transform = object["transform"];

			//平行移動
			objectData.tranlation_.x = (float)transform["translation"][1];
			objectData.tranlation_.y = (float)transform["translation"][2];
			objectData.tranlation_.z = -(float)transform["translation"][0];

			//回転角
			objectData.rotation_.x = -(float)ConvertToRadian( transform["rotation"][1] );
			objectData.rotation_.y = -(float)ConvertToRadian( transform["rotation"][2] );
			objectData.rotation_.z = (float)ConvertToRadian(transform["rotation"][0]);

			//スケーリング
			objectData.scaling_.x = (float)transform["scaling"][1];
			objectData.scaling_.y = (float)transform["scaling"][2];
			objectData.scaling_.z = (float)transform["scaling"][0];
		}

		//TODO: オブジェクト走査を再帰関数にまとめ、再帰関数で枝を走査する
		if (object.contains("children")) {

		}
	}

	//レベルデータに出現するモデルの読込
	for (auto& objectData : levelData->objects_) {
		//ファイル名から登録済みモデルを検索
		decltype(models)::iterator it = models.find(objectData.fileName_);

		//未読込の場合読み込む
		Model* model = Model::LoadFromOBJ(objectData.fileName_);
		models[objectData.fileName_] = model;
	}

	//レベルデータからオブジェクトを生成、配置
	for(auto& objectData : levelData->objects_){
		//ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName_);
		if (it != models.end()) { model = it->second; }
		//モデルを指定して3Dオブジェクトを生成
		Object3d* newObject = Object3d::Create();
		newObject->SetModel(model);
		//座標
		Vector3 pos;
		newObject->SetPosition(objectData.tranlation_);
		//回転角
		newObject->SetRotation(objectData.rotation_);
		//スケール
		newObject->SetScale(objectData.scaling_);

		//カメラセット
		newObject->SetCamera(camera);

		//配列に登録
		objects_.push_back(newObject);
	}
}

void LevelLoader::Finalize() {
	for (Object3d* object : objects_) {
		SafeDelete(object);
	}
}
