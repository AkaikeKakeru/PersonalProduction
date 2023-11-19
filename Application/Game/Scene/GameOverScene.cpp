/*ゲームオーバーシーン*/

#include "GameOverScene.h"

#include "Framework.h"
#include "SceneManager.h"

#include <Quaternion.h>
#include "SafeDelete.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

DirectXBasis* GameOverScene::dxBas_ = DirectXBasis::GetInstance();
Input* GameOverScene::input_ = Input::GetInstance();
SpriteBasis* GameOverScene::spriteBas_ = SpriteBasis::GetInstance();

void GameOverScene::Initialize() {

	/// 描画初期化
#ifdef _DEBUG
	//imGui
	imGuiManager_ = ImGuiManager::GetInstance();
#endif

	//オブジェクト基盤

	//オブジェクトモデル

	//カメラ生成
	camera_ = new Camera();

	planeModel_ = new Model();
	planeModel_ = Model::LoadFromOBJ("planeEnemy", true);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome", false);

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
	float textSize = 5.0f;

	text_ = new Text();
	text_->Initialize(Framework::kTextTextureIndex_);
	text_->SetString("GAME OVER");
	text_->SetPosition({
		WinApp::Win_Width / 2,
		WinApp::Win_Height / 2,
		});
	text_->SetSize({ textSize,textSize });

	//ボタン
	textSize = 2.5f;

	buttonTitle_ = new Button();
	buttonTitle_->Initialize(0);
	buttonTitle_->SetTelop("Title");
	buttonTitle_->SetPosition({ 300.0f ,500.0f });
	buttonTitle_->SetSize({ 400.0f,96.0f });
	buttonTitle_->GetText()->SetSize({ textSize,textSize });

	buttonRetry_ = new Button();
	buttonRetry_->Initialize(0);
	buttonRetry_->SetTelop("Retry");
	buttonRetry_->SetPosition({ WinApp::Win_Width - 300.0f ,500.0f });
	buttonRetry_->SetSize({ 400.0f,96.0f });
	buttonTitle_->GetText()->SetSize({ textSize,textSize });

	//暗幕
	blackOut_ = new Fade();
	blackOut_->Initialize(Framework::kWhiteTextureIndex_);
	blackOut_->SetSize({ WinApp::Win_Width,WinApp::Win_Height });
	blackOut_->SetColor({0,0,0,0});

	//タイルならべ

	//タイルサイズ
	float tileSize = WinApp::Win_Width / 8.0f;

	//横に並べる枚数
	float width = (WinApp::Win_Width / tileSize) + 1;
	//縦に並べる枚数
	float height = (WinApp::Win_Height / tileSize) + 1;

	arrangeTile_ = new ArrangeTile();
	arrangeTile_->Initialize(
		Framework::kBackgroundTextureIndex_,
		//開始位置
		{
			WinApp::Win_Width / 2,
			-200.0f
		},
		0.0f,
		{
			tileSize,
			tileSize
		},
		(int)(width * height)
	);
}

void GameOverScene::Update() {
	input_->Update();

	buttonTitle_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	buttonRetry_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	if (buttonTitle_->ChackClick(input_->PressMouse(0))) {
		buttonTitle_->SetColor({ 0.4f,0.4f,0.4f,1.0f });
	}
	else if (buttonRetry_->ChackClick(input_->PressMouse(0))) {
		buttonRetry_->SetColor({ 0.4f,0.4f,0.4f,1.0f });
	}

	if (buttonTitle_->ChackClick(input_->ReleaseMouse(0))) {
		////シーンの切り替えを依頼
		//SceneManager::GetInstance()->ChangeScene("TITLE");

		blackOut_->SetIs(true);
		blackOut_->SetIsOpen(false);

	}
	else if (buttonRetry_->ChackClick(input_->ReleaseMouse(0))) {
		////シーンの切り替えを依頼
		//SceneManager::GetInstance()->ChangeScene("GAMEPLAY");


		arrangeTile_->Reset(true, false);
	}

	light_->Update();

	skydomeObj_->Update();
	planeObj_->Update();

	buttonRetry_->Update();
	buttonTitle_->Update();

	sprite_->Update();

	BlackOutUpdate();
}

void GameOverScene::Draw() {
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

	text_->Print();

	text_->DrawAll();

	blackOut_->Draw();
	arrangeTile_->Draw();

	SpriteBasis::GetInstance()->PostDraw();
}

void GameOverScene::Finalize() {
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

	blackOut_->Finalize();
	SafeDelete(blackOut_);
	SafeDelete(arrangeTile_);
}

void GameOverScene::BlackOutUpdate() {
	if (blackOut_->Is()) {
		blackOut_->Update();
	}

	if (blackOut_->IsEnd()) {
		if (!blackOut_->IsOpen()) {

			//シーンの切り替えを依頼
			SceneManager::GetInstance()->ChangeScene("TITLE");
		}
	}

	if (!arrangeTile_->IsOpen()) {
		arrangeTile_->Update();
	}

	if (arrangeTile_->IsEnd()) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}
