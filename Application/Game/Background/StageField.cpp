/*
* StageField.cpp
* ステージ用のフィールド
*/

#include "StageField.h"
#include "DirectXBasis.h"
#include "Framework.h"

#include <cassert>
#include <sstream>
#include <iomanip>
#include <Quaternion.h>

void StageField::Initialize() {
    levelLoader_= new LevelLoader();
    levelLoader_->LoadFileJson(jsonFileName_, camera_);

#pragma region Skydome
	Skydome* newSkydome = Skydome::Create();
	skydome_.reset(newSkydome);
	skydome_->SetCamera(camera_);
	skydome_->SetModel(objManager_->GetModel(skydomeModel_));
	skydome_->SetScale({ 1024.0f, 256.0f, 1024.0f });
	skydome_->SetPosition({ 0,0,0 });
	skydome_->SetColor({ 0,0,0,1.0f });
#pragma endregion

	for(auto& loadObj : levelLoader_->GetObjects()) {
		Object3d* newTube = Object3d::Create();
		newTube->Initialize();

		newTube->SetScale(loadObj->GetScale());
		newTube->SetRotation(loadObj->GetRotation());
		newTube->SetPosition(loadObj->GetPosition());

		newTube->SetModel(loadObj->GetModel());

		newTube->SetCamera(camera_);
		newTube->Update();
	}

#pragma region 扉
	Object3d* obj = new Object3d();

	std::unique_ptr<Object3d> newObj =
		std::make_unique<Object3d>();

	doorPos_ = { 0,0,800 };

	obj = Object3d::Create();
	obj->SetCamera(camera_);
	obj->SetScale({ 50,400,10 });
	Vector3 scaDoorL = obj->GetScale();

	obj->SetPosition({ -scaDoorL.x, scaDoorL.y / 2,doorPos_.z });
	obj->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	obj->SetModel(objManager_->GetModel(doorModel_));

	newObj.reset(obj);
	objs_.push_back(std::move(newObj));

	obj = Object3d::Create();
	obj->SetCamera(camera_);
	obj->SetScale({ 50,400,10 });
	Vector3 scaDoorR = obj->GetScale();
	obj->SetPosition({ scaDoorR.x, scaDoorR.y / 2, doorPos_.z });
	obj->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	obj->SetModel(objManager_->GetModel(doorModel_));
	newObj.reset(obj);
	objs_.push_back(std::move(newObj));
#pragma endregion

	obj = Object3d::Create();
	obj->SetModel(objManager_->GetModel(bottomBGModel_));
	obj->SetScale({ 10.0f, 10.0f, 10.0f });
	obj->SetPosition({ 0,-100,0 });
	obj->SetCamera(camera_);
	obj->Update();

	newObj.reset(obj);
	objs_.push_back(std::move(newObj));
}

void StageField::Update() {
	skydome_->Update();

	for (Object3d*& tube : objects_) {
		tube->Update();
	}

	for (auto& tube : defaultObjects_) {
		tube->Update();
	}

	for (std::unique_ptr<Object3d>& obj : objs_) {
		obj->Update();
	}
}

void StageField::Draw() {
	//天球描画
	skydome_->Draw();

	for (auto& tube : defaultObjects_) {
		tube->Draw();
	}

	for (std::unique_ptr<Object3d>& obj : objs_) {
		obj->Draw();
	}
}

void StageField::Finalize() {
	objects_.clear();
    levelLoader_->Finalize();
	skydome_->Finalize();
}

void StageField::AddTube(/*
	const Vector3 pos,
	const Vector3 rota,
	const Vector3 scale*/) {
	for(auto& loadObj : levelLoader_->GetObjects()) {
		//Vector3 pos = loadObj->GetPosition();

		//loadObj->SetPosition({ pos.x,pos.y,kTubePopPosZ_ });

		Object3d* newTube = Object3d::Create();
		newTube->Initialize();

		newTube->SetScale(loadObj->GetScale());
		newTube->SetRotation(loadObj->GetRotation());
		newTube->SetPosition(loadObj->GetPosition());

		newTube->SetModel(loadObj->GetModel());

		newTube->SetCamera(camera_);
		newTube->Update();

		objects_.push_back(newTube);
	}
}
