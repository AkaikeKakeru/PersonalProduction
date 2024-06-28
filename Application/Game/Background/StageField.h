/*
* StageField.h
* ステージ用のフィールド
*/

#pragma once

#include "Model.h"
#include "Object3d.h"

#include "LevelLoader.h"
#include <map>
#include <vector>

struct LevelData;

class StageField {
public: //クラス内定数
    const float kTubePopPosZ_ = 600.0f;
    const float kTubeDeadPosZ_ = -300.0f;
    const int32_t kRePopTime_ = 10;
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
    Camera* camera_;

    //デフォルトオブジェクト
    std::list<Object3d*> defaultObjects_;

    //オブジェクトデータコンテナ
    std::list<Object3d*> objects_;

    //レベルローダー
    LevelLoader* levelLoader_;

    //読み込むjson名
    std::string jsonFileName_;

    //スピード
    //float speed_ = 0.0f;

    //再配置タイマー
    int32_t timerRePop_ = 0;

    //停止フラグ
    bool isStop_ = false;
};
