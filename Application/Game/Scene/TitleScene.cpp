#include "TitleScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"
#include <imgui.h>

DirectXBasis* TitleScene::dxBas_ = DirectXBasis::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();
SpriteBasis* TitleScene::spriteBas_ = SpriteBasis::GetInstance();

void TitleScene::Initialize(){
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

	//描画スプライト
	sprite_->Initialize(0);

	//テキスト
	text_ = new Text();
	text_->Initialize(Framework::kTextTextureIndex_);

	//ボタン

	buttonStart_ = new Button();
	buttonStart_->Initialize(0);
	buttonStart_->SetPosition({ WinApp::Win_Width / 2 ,500.0f });
	buttonStart_->SetSize({ 600.0f,96.0f });
}

void TitleScene::Update(){
	input_->Update();

	buttonStart_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	if (buttonStart_->ChackClick(input_->PressMouse(0))) {
		buttonStart_->SetColor({ 0.4f,0.4f,0.4f,1.0f });
	}

	if (buttonStart_->ChackClick(input_->ReleaseMouse(0))) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
	}
#endif // _DEBUG

	light_->Update();

	skydomeObj_->Update();
	planeObj_->Update();

	buttonStart_->Update();

	sprite_->Update();

	float textSize = 5.0f;

	text_->Print("Personal Production",
		buttonStart_->GetPosition().x - (text_->fontWidth_ * 2.0f * 21.0f),
		WinApp::Win_Height / 4,
		textSize);

	textSize = 2.5f;

	text_->Print("Press here to Start",
		buttonStart_->GetPosition().x - (text_->fontWidth_ * 19.0f),
		buttonStart_->GetPosition().y,
		textSize);
}

void TitleScene::Draw(){
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

	buttonStart_->Draw();

	text_->DrawAll();

	SpriteBasis::GetInstance()->PostDraw();
}

void TitleScene::Finalize(){
	SafeDelete(planeObj_);
	SafeDelete(skydomeObj_);
	SafeDelete(planeModel_);
	SafeDelete(skydomeModel_);
	SafeDelete(sprite_);

	SafeDelete(light_);
	SafeDelete(camera_);

	buttonStart_->Finalize();
	SafeDelete(buttonStart_);

	SafeDelete(text_);
}