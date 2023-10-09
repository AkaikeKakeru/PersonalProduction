#include "TitleScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"
#include <imgui.h>

DirectXBasis* TitleScene::dxBas_ = DirectXBasis::GetInstance();
Input* TitleScene::input_ = Input::GetInstance();
SpriteBasis* TitleScene::spriteBas_ = SpriteBasis::GetInstance();

void TitleScene::Initialize() {
	/// 描画初期化

	//imGui
	imGuiManager_ = ImGuiManager::GetInstance();

	//オブジェクト基盤

	//オブジェクトモデル

	//カメラ生成
	camera_ = new Camera();
	camera_->SetTarget({ 0,ConvertToRadian(-90.0f),0 });

	planeModel_ = new Model();
	planeModel_ = Model::LoadFromOBJ("plane", true);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome", false);
#pragma region Player
	player_ = new Object3d();
	player_ = Object3d::Create();

	player_->SetPosition({ 0,0,-50.0f });

	player_->SetModel(planeModel_);
	player_->SetCamera(camera_);
#pragma endregion

#pragma region Skydome
	skydome_ = Skydome::Create();
	skydome_->SetModel(skydomeModel_);
	skydome_->SetScale({ 512.0f, 126.0f, 512.0f });
	skydome_->SetPosition({ 0,0,-50.0f });
	skydome_->SetCamera(camera_);
	skydome_->Update();
#pragma endregion

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	Object3d::SetLight(light_);

	//描画スプライト
	sprite_ = new Sprite();
	sprite_->Initialize(0);

	blackOut_ = new Sprite();
	blackOut_->Initialize(Framework::kWhiteTextureIndex_);
	blackOut_->SetSize({ WinApp::Win_Width,WinApp::Win_Height });
	blackOut_->SetColor(cColorBlack_);

	float textSize = 2.5f;

	//ボタン
	buttonStart_ = new Button();
	buttonStart_->Initialize(0);
	buttonStart_->SetTelop("Click here to Start");
	buttonStart_->SetPosition({ WinApp::Win_Width / 2 ,500.0f });
	buttonStart_->SetSize({ 600.0f,96.0f });
	buttonStart_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	buttonStart_->GetText()->
		SetSize({ textSize,textSize });

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

	text_ = new Text();
	text_->Initialize(Framework::kTextTextureIndex_);
	text_->SetString("Personal Production");
	text_->SetPosition({
		buttonStart_->GetPosition().x,
		WinApp::Win_Height / 4,
		});
	text_->SetSize({ textSize,textSize });
}

void TitleScene::Update() {
	input_->Update();

	alpha_ = RoopFloat(alpha_, 0.04f, 0.0f, 1.0f);

	buttonStart_->SetColor({ 1.0f,1.0f,1.0f,alpha_ });

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

	CameraUpdate();

	skydome_->Update();
	PlayerUpdate();

	buttonStart_->Update();

	sprite_->Update();

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
	player_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	SpriteBasis::GetInstance()->PreDraw();

	//sprite_->Draw();

	buttonStart_->Draw();
	text_->Print();

	text_->DrawAll();

	blackOut_->Draw();
	SpriteBasis::GetInstance()->PostDraw();
}

void TitleScene::Finalize() {
	SafeDelete(skydome_);

	SafeDelete(player_);
	SafeDelete(planeModel_);
	SafeDelete(skydomeModel_);
	SafeDelete(sprite_);

	SafeDelete(light_);
	SafeDelete(camera_);

	buttonStart_->Finalize();
	SafeDelete(buttonStart_);

	SafeDelete(text_);

	SafeDelete(blackOut_);
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
	Vector3 pos = player_->GetPosition();
	Vector3 move{ 0,0,0 };

	if (player_->GetPosition().z >= 0.0f) {
		move = { 0,0,0 };
	}
	else {
		move = { 0,0,0.4f };
	}

	pos += move;

	player_->SetPosition(pos);

	player_->Update();
}

void TitleScene::BlackOutUpdate() {
	Vector4 fade = {cColorBlack_};
	float alpha = blackOut_->GetColor().w;

	float speed = 0.1f;

	//ループタイマーのカウント
	if (roopTimer_ < 0) {
		isRoop_ = true;
	}
	else {
		roopTimer_--;
	}

	//ループフラグが立ったら、暗幕を可視化させていく
	if (isRoop_) {
		speed = 0.1f;
		alpha += speed;

		if (alpha >= 1.0f) {
			//シーンの切り替えを依頼
			SceneManager::GetInstance()->ChangeScene("TITLE");

			speed = 0.0f;
			alpha = 1.0f;
		}
	}
	//立っていなければ、透明化させていく
	else {
		speed = -0.1f;
		alpha += speed;

		if (alpha <= 0.0f) {
			speed = 0.0f;
			alpha = 0.0f;
		}
	}

	fade.w = alpha;

	blackOut_->SetColor(fade);

	blackOut_->Update();
}

float TitleScene::RoopFloat(float f, float speed, float min, float max) {
	static float t = speed;

	if (f < min || f > max) {
		t *= -1.0f;
	}

	f += t;

	return f;
}
