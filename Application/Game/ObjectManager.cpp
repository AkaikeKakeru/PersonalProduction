#include "ObjectManager.h"

//モデルリスト
std::vector<std::unique_ptr<Model>> ObjectManager::models_;

//オブジェリスト
std::list<std::unique_ptr<Object3d>> ObjectManager::objs_;

void ObjectManager::CreateModelList() {
#pragma region modelLoad
	std::unique_ptr<Model> newModel =
		std::make_unique<Model>();
	newModel.reset(Model::LoadFromOBJ("human", true));
	models_.push_back(std::move(newModel));
	newModel.reset(Model::LoadFromOBJ("human-hide", true));
	models_.push_back(std::move(newModel));
	newModel.reset(Model::LoadFromOBJ("planeEnemy", true));
	models_.push_back(std::move(newModel));
	newModel.reset(Model::LoadFromOBJ("skydome", false));
	models_.push_back(std::move(newModel));
	newModel.reset(Model::LoadFromOBJ("missile", true));
	models_.push_back(std::move(newModel));
	newModel.reset(Model::LoadFromOBJ("BG_Tube", true));
	models_.push_back(std::move(newModel));
	newModel.reset(Model::LoadFromOBJ("cart", true));
	models_.push_back(std::move(newModel));
	newModel.reset(Model::LoadFromOBJ("bottom", true));
	models_.push_back(std::move(newModel));
	newModel.reset(Model::LoadFromOBJ("clearBottom", true));
	models_.push_back(std::move(newModel));
	newModel.reset(Model::LoadFromOBJ("cube", true));
	models_.push_back(std::move(newModel));
#pragma endregion
}

void ObjectManager::CreateObjectList() {

}

ObjectManager::~ObjectManager() {
}

ObjectManager* ObjectManager::GetInstance() {
	static ObjectManager instance;
	return &instance;
}
