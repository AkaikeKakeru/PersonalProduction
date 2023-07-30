#include "GameClearScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"
#include <imgui.h>

DirectXBasis* GameClearScene::dxBas_ = DirectXBasis::GetInstance();
Input* GameClearScene::input_ = Input::GetInstance();
SpriteBasis* GameClearScene::spriteBas_ = SpriteBasis::GetInstance();

void GameClearScene::Initialize(){
	/// 描画初期化

	//imGui
	imGuiManager_ = ImGuiManager::GetInstance();

	//オブジェクト基盤

	//オブジェクトモデル

	//カメラ生成
	camera_ = new Camera();

	planeModel_ = new Model();
	planeModel_ = Model::LoadFromOBJ("plane", false);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome",false);

	planeObj_ = new Object3d();
	planeObj_ = Object3d::Create();
	planeObj_->SetModel(planeModel_);
	planeObj_->SetCamera(camera_);

	skydomeObj_ = new Object3d();
	skydomeObj_ = Object3d::Create();
	skydomeObj_->SetModel(skydomeModel_);
	skydomeObj_->SetCamera(camera_);

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	Object3d::SetLight(light_);

	//描画基盤

	//描画スプライト
	sprite_ = new Sprite();
	sprite_->Initialize(0);

	//テキスト
	text_ = new Text();
	text_->Initialize(Framework::kTextTextureIndex_);
}

void GameClearScene::Update(){
	input_->Update();

	light_->Update();

	skydomeObj_->Update();
	planeObj_->Update();

	sprite_->Update();

	if (input_->TriggerKey(DIK_BACK)) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	else if (input_->TriggerKey(DIK_RETURN)) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	text_->Print("GAME CLEAR!",
		WinApp::Win_Width/4 + text_->fontWidth_ * 6.5f,
		WinApp::Win_Height/2,
		5.0f);
}

void GameClearScene::Draw(){
#ifdef _DEBUG
	imGuiManager_->Begin();

	imGuiManager_->End();
#endif

	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());

	skydomeObj_->Draw();
	planeObj_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	SpriteBasis::GetInstance()->PreDraw();

	sprite_->Draw();

	text_->DrawAll();

	SpriteBasis::GetInstance()->PostDraw();
}

void GameClearScene::Finalize(){
	SafeDelete(planeObj_);
	SafeDelete(skydomeObj_);
	SafeDelete(planeModel_);
	SafeDelete(skydomeModel_);
	SafeDelete(sprite_);

	SafeDelete(light_);
	SafeDelete(camera_);
	SafeDelete(text_);
}