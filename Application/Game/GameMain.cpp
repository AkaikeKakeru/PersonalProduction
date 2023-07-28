#include "GameMain.h"
#include "SceneFactory.h"

SceneManager* GameMain::sceneManager_ = Framework::GetSceneManager();

void GameMain::Initialize() {
	Framework::Initialize();

	sceneManager_ = Framework::GetSceneManager();

	//シーンファクトリーを生成し、マネージャーにセット
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);

	//シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("TITLE");
}

void GameMain::Update() {
	Framework::Update();
}

void GameMain::Draw() {
	//描画前処理
	Framework::GetDirectXBasis()->PreDraw();

	sceneManager_ = Framework::GetSceneManager();
	sceneManager_->Draw();

	//描画後処理
	Framework::GetDirectXBasis()->PostDraw();
}

void GameMain::Finalize() {
	Framework::Finalize();
}