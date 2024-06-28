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

void StageField::Initialize() {
    levelLoader_= new LevelLoader();
    levelLoader_->LoadFileJson(jsonFileName_, camera_);

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

		defaultObjects_.push_back(newTube);
	}
}

void StageField::Update() {

	//Vector3 speed{ 0,0,-16.0f };

	//timerRePop_++;

	//if (!isStop_) {

	//	if (timerRePop_ >= kRePopTime_) {
	//		AddTube();

	//		timerRePop_ = 0;
	//	}
	//}

	for (Object3d*& tube : objects_) {
		//Vector3 pos = tube->GetPosition();

		//pos += speed;

		//tube->SetPosition(pos);

		tube->Update();

		//if (tube->GetPosition().z <= kTubeDeadPosZ_) {
		//	objects_.remove(tube);
		//}
	}

	//objects_.remove_if([](Object3d*& obj) {
	//	return obj->GetPosition().z <= -300.0f;
	//	});

	for (auto& tube : defaultObjects_) {
		tube->Update();
	}
}

void StageField::Draw() {
	//for (auto& tube : objects_) {
	//	tube->Draw();
	//}
	for (auto& tube : defaultObjects_) {
		tube->Draw();
	}
}

void StageField::Finalize() {
	objects_.clear();
	defaultObjects_.clear();
    levelLoader_->Finalize();
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
