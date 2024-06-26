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
}

void StageField::Update() {
    for(auto& block : levelLoader_->GetObjects()) {
        block->Update();
    }
}

void StageField::Draw() {
    for(auto& block : levelLoader_->GetObjects()) {
        block->Draw();
    }
}

void StageField::Finalize() {
    for (Object3d*& block : levelLoader_->GetObjects()) {
        delete block;
    }

    levelLoader_->Finalize();
}
