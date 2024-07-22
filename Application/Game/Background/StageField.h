/*
* StageField.h
* ステージ用のフィールド
*/

#pragma once

#include "Model.h"
#include "Object3d.h"

#include "ObjectManager.h"
#include "LevelLoader.h"
#include <map>
#include <vector>

#include "Skydome.h"

struct LevelData;

class StageField {
public: //クラス内定数
    const float kTubePopPosZ_ = 600.0f;
    const float kTubeDeadPosZ_ = -300.0f;
    const int32_t kRePopTime_ = 10;

public:
    enum modelName {
        skydomeModel_ = ObjectManager::skydomeModel_,
        tubeModel_ = ObjectManager::tubeModel_,
        bottomBGModel_ = ObjectManager::bottomBGModel_,
        doorModel_ = ObjectManager::doorModel_,
    };

    enum objName {
        doorL_,
        doorR_,
        bottomBG_,
    };

public:
    void Initialize();
    void Update();
    void Draw();
    void Finalize();

    void AddTube(/*
        const Vector3 pos,
        const Vector3 rota,
        const Vector3 scale*/);

public:
    void SetCamera(Camera* camera) {
        camera_ = camera;
    };

    std::vector<Object3d*> GetObjectVector() {
        return levelLoader_->GetObjects();
    }

    void SetJsonFileName(const std::string fileName) {
        jsonFileName_ = fileName;
    }

private:
    static ObjectManager* objManager_;

private:
    Camera* camera_;

    //デフォルトオブジェクト
    std::list<Object3d*> defaultObjects_;

    //オブジェクトデータコンテナ
    std::list<Object3d*> objects_;

    //天球
    std::unique_ptr<Skydome> skydome_ = nullptr;

    //扉の位置
    Vector3 doorPos_{};

    //オブジェリスト
    std::list<std::unique_ptr<Object3d>> objs_;

    //レベルローダー
    LevelLoader* levelLoader_;

    //読み込むjson名
    std::string jsonFileName_;
};
