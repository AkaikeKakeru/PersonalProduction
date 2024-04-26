#pragma once
#include <Model.h>

#include <memory>
#include <List>
#include <Object3d.h>

class ObjectManager {
public:
	//モデル名
	enum modelName {
		playerActiveModel_,
		playerHideModel_,
		enemyModel_,
		skydomeModel_,
		bulletModel_,
		tubeModel_,
		cartModel_,
		bottomBGModel_,
		bottomClearBGModel_,
		doorModel_,
	};

	//オブジェクト名
	enum objName {
		doorL_,
		doorR_,
		bottomBG_,
	};

public:
	void CreateModelList();

	void CreateObjectList();

	//モデル取得
	Model* GetModel(int32_t modelName) const {
		return models_[modelName].get();
	}

private:
	//モデルリスト
	static std::vector<std::unique_ptr<Model>> models_;

	//オブジェリスト
	static std::list<std::unique_ptr<Object3d>> objs_;

private:
	ObjectManager() = default;
	~ObjectManager();
	//コピーコンストラクタを無効
	ObjectManager(const ObjectManager&) = delete;
	//代入演算子を無効
	const ObjectManager& operator=(const ObjectManager&) = delete;

public://static
	static ObjectManager* GetInstance();
};
