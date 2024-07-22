/*タイトルシーン*/

#include "TitleScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"
#ifdef _DEBUG
#include <imgui.h>
#endif
#include <Character.h>
#include <Quaternion.h>

DirectXBasis* TitleScene::dxBas_ = DirectXBasis::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();
SpriteBasis* TitleScene::spriteBas_ = SpriteBasis::GetInstance();
ObjectManager* TitleScene::objManager_ = ObjectManager::GetInstance();

void TitleScene::Initialize() {
	/// 描画初期化
#ifdef _DEBUG
	//imGui
	imGuiManager_ = ImGuiManager::GetInstance();
#endif

	//オブジェクト基盤

	//オブジェクトモデル

	//カメラ生成
	camera_ = new Camera();
	camera_->SetTarget({ 0,ConvertToRadian(-90.0f),0 });
	camera_->SetEye({ 20,-10,30 });

	Object3d* newObj = new Object3d();
#pragma region Player

	newObj = Object3d::Create();
	newObj->SetCamera(camera_);
	newObj->SetPosition({ 0,0,-50.0f });
	newObj->SetModel(objManager_->GetModel(playerModel_));

	playerObj_.reset(newObj);
#pragma endregion

#pragma region cart
	newObj = Object3d::Create();
	newObj->SetCamera(camera_);
	newObj->SetPosition({
		playerObj_->GetPosition().x,
		playerObj_->GetPosition().y - 2.5f,
		playerObj_->GetPosition().z }
	);
	newObj->SetModel(objManager_->GetModel(cartModel_));

	cart_.reset(newObj);
#pragma endregion

#pragma region Skydome
	Skydome* newSky = new Skydome();
	newSky = Skydome::Create();
	newSky->SetModel(objManager_->GetModel(skydomeModel_));
	newSky->SetScale({ 1024.0f, 126.0f, 1024.0f });
	newSky->SetPosition({ 0,0,-50.0f });
	newSky->SetCamera(camera_);
	newSky->Update();

	skydome_.reset(newSky);
#pragma endregion

	newObj = Object3d::Create();
	newObj->SetCamera(camera_);
	newObj->SetModel(objManager_->GetModel(bottomBGModel_));
	newObj->SetScale({ 10.0f, 10.0f, 10.0f });
	newObj->SetPosition({ 0,-100,0 });
	newObj->Update();

	bottomBG_.reset(newObj);

	stageField_ = std::make_unique<StageField>();
	stageField_->SetJsonFileName("StageField1");
	stageField_->SetCamera(camera_);
	stageField_->Initialize();

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	Object3d::SetLight(light_);

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

	float textSize = 2.5f;

	//ボタン
	Button* newButton = new Button();
	newButton->Initialize(0);
	newButton->SetTelop("Click here to Start");
	newButton->SetPosition({ WinApp::Win_Width / 2 ,500.0f });
	newButton->SetSize({ 600.0f,96.0f });
	newButton->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	newButton->GetText()->
		SetSize({ textSize,textSize });

	buttonStart_.reset(newButton);

	float len =
		static_cast<float>(
			buttonStart_->GetText()->GetString().length()
			);

	buttonStart_->GetText()->
		SetPosition({
		WinApp::Win_Width / 2.0f + len * 8.0f,
		500.0f
			});

	//テキスト
	textSize = 5.0f;

	Text* newText = new Text();
	newText->Initialize(Framework::kTextTextureIndex_);
	newText->SetString("Remain");
	newText->SetPosition({
		buttonStart_->GetPosition().x,
		WinApp::Win_Height / 4,
		});
	newText->SetSize({ textSize,textSize });

	text_.reset(newText);
}

void TitleScene::Update() {
	input_->Update();

	alpha_ = RoopFloat(alpha_, 0.04f, 0.0f, 1.0f);

	buttonStart_->SetColor({ 1.0f,1.0f,1.0f,alpha_ });

	if (buttonStart_->ChackClick(input_->PressMouse(0))) {
		buttonStart_->SetColor({ 0.4f,0.4f,0.4f,1.0f });
	}

	if (buttonStart_->ChackClick(input_->ReleaseMouse(0))) {
		////シーンの切り替えを依頼
		//SceneManager::GetInstance()->ChangeScene("GAMEPLAY");

		blackOut_->SetIs(true);
		blackOut_->SetIsOpen(false);

		arrangeTile_->Reset(true, false);
	}

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
	}
#endif // _DEBUG

	light_->Update();

	CameraUpdate();

	skydome_->Update();
	bottomBG_->Update();

	stageField_->Update();

	PlayerUpdate();

	buttonStart_->Update();

	BlackOutUpdate();
}

void TitleScene::Draw() {
#ifdef _DEBUG
	imGuiManager_->Begin();

	imGuiManager_->End();
#endif

	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());
	//天球描画
	skydome_->Draw();
	bottomBG_->Draw();

	stageField_->Draw();

	playerObj_->Draw();
	cart_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	SpriteBasis::GetInstance()->PreDraw();

	buttonStart_->Draw();
	text_->Print();

	text_->DrawAll();

	blackOut_->Draw();
	arrangeTile_->Draw();
	SpriteBasis::GetInstance()->PostDraw();
}

void TitleScene::Finalize() {
	stageField_->Finalize();
	SafeDelete(light_);
	SafeDelete(camera_);
	buttonStart_->Finalize();
	blackOut_->Finalize();
}

void TitleScene::CameraUpdate() {
	Vector3 target = camera_->GetTarget();
	Vector3 move{ 0,0,0 };

	if (camera_->GetTarget().y >= 0) {
		move = { 0,0,0 };
	}
	else {
		move = { 0,ConvertToRadian(1.5f),0 };
	}

	target += move;

	camera_->SetTarget(target);

	camera_->Update();
}

void TitleScene::PlayerUpdate() {
	Vector3 pos = playerObj_->GetPosition();
	Vector3 move{ 0,0,0 };

	if (playerObj_->GetPosition().z >= 0.0f) {
		move = { 0,0,0 };
	}
	else {
		move = { 0,0,0.4f };
	}

	move.y = RoopFloat(move.y, 0.1f, -1.0f, 1.0f);

	pos += move;

	playerObj_->SetPosition(pos);

	playerObj_->Update();

	cart_->SetPosition({
		playerObj_->GetPosition().x,
		-15.0f,
		playerObj_->GetPosition().z }
	);

	cart_->Update();
}

void TitleScene::BlackOutUpdate() {
	blackOut_->Update();

	if (!arrangeTile_->IsOpen()) {
		arrangeTile_->Update();
	}

	if (arrangeTile_->IsEnd()) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}

float TitleScene::RoopFloat(float f, float speed, float min, float max) {
	static float t = speed;

	if (f < min || f > max) {
		t *= -1.0f;
	}

	f += t;

	return f;
}
