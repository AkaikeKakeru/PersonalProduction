#pragma once

#include "json.hpp"

#include <string>

#include <vector>
#include <Vector3.h>
#include <Object3d.h>
#include <Model.h>

class LevelLoader {
public:
	// 構造体
	struct LevelData {
		struct ObjectData {
			std::string fileName_; //オブジェクトファイルネーム
			Vector3 tranlation_; //平行移動
			Vector3 rotation_; //回転
			Vector3 scaling_; //スケーリング
		};
		//オブジェクトデータコンテナ
		std::vector<ObjectData> objects_;
	};

public:
	void LoadFileJson(const std::string fileName,Camera* camera);

	void Finalize();

	std::vector<Object3d*> GetObjects() {
		return objects_;
	}

private:
	static const std::string kDefaultBaseDirectory_;

	//モデルデータコンテナ
	std::map <std::string, Model*> models;

	//オブジェクトデータコンテナ
	std::vector<Object3d*> objects_;
};