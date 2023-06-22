#include "SceneManager.h"
#include "ImGuiManager.h"
#include <cassert>

void SceneManager::Update(){
	//次のシーンの予約があるなら
	if (nextScene_) {
		//旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}

		//シーン切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;

		//シーンマネージャをセット
		scene_->SetSceneManager(this);

		//次のシーンを初期化
		scene_->Initialize();
	}

	//実行中シーンの更新
	scene_->Update();
}

void SceneManager::Draw(){
	scene_->Draw();
#ifdef _DEBUG
	ImGuiManager::GetInstance()->Draw();
#endif // DEBUG
}

void SceneManager::Finalize(){
	//最後のシーンの終了と解放
	scene_->Finalize();
	delete scene_;
}

void SceneManager::ChangeScene(const std::string& sceneName) {
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	//次シーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

SceneManager* SceneManager::GetInstance(){
	static SceneManager instance ;

	return &instance;
}