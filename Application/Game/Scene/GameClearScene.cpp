#include "GameClearScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"
#include <imgui.h>
#include <Quaternion.h>

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
	planeModel_ = Model::LoadFromOBJ("plane", true);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome",false);

	planeObj_ = new Object3d();
	planeObj_ = Object3d::Create();
	planeObj_->SetModel(planeModel_);
	planeObj_->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(180.0f)));
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

	//ボタン

	buttonTitle_ = new Button();
	buttonTitle_->Initialize(0);
	buttonTitle_->SetPosition({ 300.0f ,500.0f });
	buttonTitle_->SetSize({ 400.0f,96.0f });

	buttonRetry_ = new Button();
	buttonRetry_->Initialize(0);
	buttonRetry_->SetPosition({WinApp::Win_Width - 300.0f ,500.0f });
	buttonRetry_->SetSize({ 400.0f,96.0f });
}

void GameClearScene::Update(){
	input_->Update();

	buttonTitle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	buttonRetry_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	if (buttonTitle_->ChackClick(input_->PressMouse(0))) {
		buttonTitle_->SetColor({ 0.4f,0.4f,0.4f,1.0f });
	}
	else if (buttonRetry_->ChackClick(input_->PressMouse(0))){
		buttonRetry_->SetColor({ 0.4f,0.4f,0.4f,1.0f });
	}

	if (buttonTitle_->ChackClick(input_->ReleaseMouse(0))) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	else if (buttonRetry_->ChackClick(input_->ReleaseMouse(0))){
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	light_->Update();

	skydomeObj_->Update();
	planeObj_->Update();

	buttonRetry_->Update();
	buttonTitle_->Update();

	sprite_->Update();

	float textSize = 5.0f;

	text_->Print("Title",
		buttonTitle_->GetPosition().x - (text_->fontWidth_ * 2.0f * 5.0f),
		buttonTitle_->GetPosition().y,
		textSize);

	text_->Print("Retry",
		buttonRetry_->GetPosition().x - (text_->fontWidth_ * 2.0f * 5.0f),
		buttonRetry_->GetPosition().y,
		textSize);

	text_->Print("GAME CLEAR!",
		WinApp::Win_Width / 2 - (text_->fontWidth_ * 2.0f * 11.0f),
		WinApp::Win_Height / 2,
		textSize);
}

void GameClearScene::Draw(){
#ifdef _DEBUG
	imGuiManager_->Begin();

	imGuiManager_->End();
#endif

	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());

	//skydomeObj_->Draw();
	planeObj_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	SpriteBasis::GetInstance()->PreDraw();

	//sprite_->Draw();

	buttonRetry_->Draw();
	buttonTitle_->Draw();

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

	buttonRetry_->Finalize();
	SafeDelete(buttonRetry_);

	buttonTitle_->Finalize();
	SafeDelete(buttonTitle_);

	SafeDelete(text_);
}