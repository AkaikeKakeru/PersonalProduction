/*
* StageField.h
* ステージ用のフィールド
*/

#pragma once

#include "Model.h"
#include "Object3d.h"

#include "LevelLoader.h"
#include <map>

struct LevelData;

class StageField {
public:
    static const int cBlockCount_ = 100;

public:
    void Initialize();
    void Update();
    void Draw();
    void Finalize();

public:
    void SetCamera(const Camera& camera) {
        Camera c = camera;

        camera_ = &c;
    };

    std::vector<Object3d*> GetObjectVector() {
        return levelLoader_->GetObjects();
    }

    void SetJsonFileName(const std::string fileName) {
        jsonFileName_ = fileName;
    }

private:
    Camera* camera_;

    //レベルローダー
    LevelLoader* levelLoader_;

    //読み込むjson名
    std::string jsonFileName_;
};
