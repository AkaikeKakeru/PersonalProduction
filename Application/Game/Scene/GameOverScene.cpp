﻿#include "GameOverScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"
#include <imgui.h>

DirectXBasis* GameOverScene::dxBas_ = DirectXBasis::GetInstance();
Input* GameOverScene::input_ = Input::GetInstance();

void GameOverScene::Initialize(){
	/// 描画初期化

	//imGui
	imGuiManager_ = ImGuiManager::GetInstance();

	//オブジェクト基盤
	//Object3d::StaticInitialize(dxBas_->GetDevice().Get());

	//オブジェクトモデル

	//カメラ生成
	camera_ = new Camera();

	//planeModel_ = new Model();
	//planeModel_ = Model::LoadFromOBJ("plane", false);

	//skydomeModel_ = new Model();
	//skydomeModel_ = Model::LoadFromOBJ("skydome",false);


	//planeObj_ = new Object3d();
	//planeObj_ = Object3d::Create();
	//planeObj_->SetModel(planeModel_);
	//planeObj_->SetCamera(camera_);

	//skydomeObj_ = new Object3d();
	//skydomeObj_ = Object3d::Create();
	//skydomeObj_->SetModel(skydomeModel_);
	//skydomeObj_->SetCamera(camera_);

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	//Object3d::SetLight(light_);

	////描画基盤
	//drawBas_ = DrawBasis::GetInstance();
	//drawBas_->Initialize();

	//drawBas_->LoadTexture(0, "texture.png");

	////描画スプライト

	//sprite_->Initialize(drawBas_,0);
}

void GameOverScene::Update(){
	input_->Update();

	light_->Update();

	//skydomeObj_->Update();
	//planeObj_->Update();

	//sprite_->Update();

	if (input_->TriggerKey(DIK_BACK)) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	else if (input_->TriggerKey(DIK_RETURN)) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}

void GameOverScene::Draw(){
#ifdef _DEBUG
	imGuiManager_->Begin();

	imGuiManager_->End();
#endif

	////モデル本命処理
	//Object3d::PreDraw(dxBas_->GetCommandList().Get());

	//skydomeObj_->Draw();
	//planeObj_->Draw();

	//Object3d::PostDraw();

	////スプライト本命処理
	//drawBas_->PreDraw();

	//sprite_->Draw();

	//drawBas_->PostDraw();
}

void GameOverScene::Finalize(){
	/*SafeDelete(planeObj_);
	SafeDelete(skydomeObj_);
	SafeDelete(planeModel_);
	SafeDelete(skydomeModel_);
	SafeDelete(sprite_);*/

	SafeDelete(light_);
	SafeDelete(camera_);
}