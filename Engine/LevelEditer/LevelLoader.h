/*レベルエディター*/
#pragma once

#include "json.hpp"
#include <string>
#include <vector>
#include <Vector3.h>
#include <Object3d.h>
#include <Model.h>



/*レベルエディター*/
class LevelLoader {
public:
	//デフォルトディレクトリ
	static const std::string kDefaultBaseDirectory_;
	// ファイル拡張子
	static const std::string kExtension;
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

	//Jsonファイルの読込
	void LoadFileJson(const std::string fileName,Camera* camera);

	void Finalize();

public:
	//オブジェクトの取得
	std::vector<Object3d*> GetObjects() {
		return objects_;
	}

private:
	//モデルデータコンテナ
	std::map <std::string, Model*> models_;

	//オブジェクトデータコンテナ
	std::vector<Object3d*> objects_;
};
