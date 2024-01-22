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
ObjectManager* GameOverScene::objManager_ = ObjectManager::GetInstance();

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
	Object3d* newObj = new Object3d();

	newObj = Object3d::Create();
	newObj->SetModel(objManager_->GetModel(enemyModel_));
	newObj->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(180.0f)));
	newObj->SetCamera(camera_);
	planeObj_.reset(newObj);

	newObj = Object3d::Create();
	newObj->SetModel(objManager_->GetModel(skydomeModel_));
	newObj->SetCamera(camera_);
	skydomeObj_.reset(newObj);

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	Object3d::SetLight(light_);

	//描画基盤

	//テキスト
	float textSize = 5.0f;

	Text* newText = new Text();
	newText->Initialize(Framework::kTextTextureIndex_);
	newText->SetString("GAME OVER");
	newText->SetPosition({
		WinApp::Win_Width / 2,
		WinApp::Win_Height / 2,
		});
	newText->SetSize({ textSize,textSize });
	text_.reset(newText);

	//ボタン
	textSize = 2.5f;
	Button* newButton = new Button();

	newButton->Initialize(0);
	newButton->SetTelop("Title");
	newButton->SetPosition({ 300.0f ,500.0f });
	newButton->SetSize({ 400.0f,96.0f });
	newButton->GetText()->SetSize({ textSize,textSize });
	buttonTitle_.reset(newButton);

	Button* newButton2 = new Button();
	newButton2->Initialize(0);
	newButton2->SetTelop("Retry");
	newButton2->SetPosition({ WinApp::Win_Width - 300.0f ,500.0f });
	newButton2->SetSize({ 400.0f,96.0f });
	newButton2->GetText()->SetSize({ textSize,textSize });
	buttonRetry_.reset(newButton2);

	//暗幕
	Fade* newFade = new Fade();
	newFade->Initialize(Framework::kWhiteTextureIndex_);
	newFade->SetSize({ WinApp::Win_Width,WinApp::Win_Height });
	newFade->SetColor({0,0,0,1});
	
	blackOut_.reset(newFade);
	//タイルならべ

	//タイルサイズ
	float tileSize = WinApp::Win_Width / 8.0f;

	//横に並べる枚数
	float width = (WinApp::Win_Width / tileSize) + 1;
	//縦に並べる枚数
	float height = (WinApp::Win_Height / tileSize) + 1;

	ArrangeTile* newArrTile = new ArrangeTile();
	newArrTile->Initialize(
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

	arrangeTile_.reset(newArrTile);
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
		blackOut_->SetIs(true);
		blackOut_->SetIsOpen(false);

	}
	else if (buttonRetry_->ChackClick(input_->ReleaseMouse(0))) {
		arrangeTile_->Reset(true, false);
	}

	light_->Update();

	skydomeObj_->Update();
	planeObj_->Update();

	buttonRetry_->Update();
	buttonTitle_->Update();

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

	buttonRetry_->Draw();
	buttonTitle_->Draw();

	text_->Print();

	text_->DrawAll();

	blackOut_->Draw();
	arrangeTile_->Draw();

	SpriteBasis::GetInstance()->PostDraw();
}

void GameOverScene::Finalize() {
	SafeDelete(light_);
	SafeDelete(camera_);

	buttonRetry_->Finalize();
	buttonTitle_->Finalize();
	blackOut_->Finalize();
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
