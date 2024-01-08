/*ゲームクリアシーン*/

#include "GameClearScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"
#include <Quaternion.h>

#ifdef _DEBUG
#include <imgui.h>
#endif

DirectXBasis* GameClearScene::dxBas_ = DirectXBasis::GetInstance();
Input* GameClearScene::input_ = Input::GetInstance();
SpriteBasis* GameClearScene::spriteBas_ = SpriteBasis::GetInstance();

void GameClearScene::Initialize() {
	/// 描画初期化
#ifdef _DEBUG
	//imGui
	imGuiManager_ = ImGuiManager::GetInstance();
#endif

	//オブジェクト基盤

	//オブジェクトモデル

	//カメラ生成
	camera_ = new Camera();
	camera_->SetEye({ 0, 0, 0 });
	camera_->SetTarget({ 0, 0, 0 });

	playerModel_ = new Model();
	playerModel_ = Model::LoadFromOBJ("human", true);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome", false);

	doorModel_ = new Model();
	doorModel_ = Model::LoadFromOBJ("cube", true);

	cartModel_ = new Model();
	cartModel_ = Model::LoadFromOBJ("cart",true);

	playerObj_ = new Object3d();
	playerObj_ = Object3d::Create();
	playerObj_->SetModel(playerModel_);
	playerObj_->SetPosition({ 0, -5.0f, 150.0f });
	playerObj_->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	playerObj_->SetCamera(camera_);

	camera_->SetTarget(playerObj_->GetPosition());

#pragma region cart
	cart_ = new Object3d();
	cart_ = Object3d::Create();

	cart_->SetPosition({
		playerObj_->GetPosition().x,
		playerObj_->GetPosition().y -2.5f,
		playerObj_->GetPosition().z}
	);

	cart_->SetModel(cartModel_);
	cart_->SetCamera(camera_);
#pragma endregion

	skydomeObj_ = new Object3d();
	skydomeObj_ = Object3d::Create();
	skydomeObj_->SetModel(skydomeModel_);
	skydomeObj_->SetScale({ 1024.0f, 256.0f, 1024.0f });
	skydomeObj_->SetPosition({ 0,0,0 });
	skydomeObj_->SetCamera(camera_);


	doorL_ = new Object3d();
	doorR_ = new Object3d();

	doorPos_ = { 0,0,800 };

	doorL_ = Object3d::Create();
	doorL_->SetScale({ 50,400,10 });
	Vector3 scaDoorL = doorL_->GetScale();

	doorL_->SetPosition({ -scaDoorL.x, scaDoorL.y / 2,doorPos_.z });
	doorL_->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	doorL_->SetModel(doorModel_);
	doorL_->SetCamera(camera_);

	doorR_ = Object3d::Create();
	doorR_->SetScale({ 50,400,10 });
	Vector3 scaDoorR = doorR_->GetScale();

	doorR_->SetPosition({ scaDoorR.x, scaDoorR.y / 2, doorPos_.z });
	doorR_->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	doorR_->SetModel(doorModel_);
	doorR_->SetCamera(camera_);

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
	text_->SetString("GAME CLEAR!");
	text_->SetPosition({
		WinApp::Win_Width / 2,
		-20.0f,
		});
	text_->SetSize({ textSize,textSize });

	//ボタン
	textSize = 2.5f;

	buttonTitle_ = new Button();
	buttonTitle_->Initialize(0);
	buttonTitle_->SetTelop("Title");
	buttonTitle_->SetPosition({ -200.0f ,500.0f });
	buttonTitle_->SetSize({ 400.0f,96.0f });
	buttonTitle_->GetText()->SetSize({ textSize,textSize });
	buttonTitle_->Update();

	buttonRetry_ = new Button();
	buttonRetry_->Initialize(0);
	buttonRetry_->SetTelop("Retry");
	buttonRetry_->SetPosition({ WinApp::Win_Width - 300.0f ,500.0f });
	buttonRetry_->SetSize({ 400.0f,96.0f });
	buttonRetry_->GetText()->SetSize({ textSize,textSize });
	buttonRetry_->Update();

	//暗幕
	blackOut_ = new Fade();
	blackOut_->Initialize(Framework::kWhiteTextureIndex_);
	blackOut_->SetSize({ WinApp::Win_Width,WinApp::Win_Height });
	blackOut_->SetColor({ 0,0,0,1 });

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

	const int timeMax = 60;
	Vector2 start =
		buttonTitle_->GetPosition();
	Vector2 end = {300.0f,buttonTitle_->GetPosition().y};

	easeButtonPosition_.Reset(
		Ease::In_,
		timeMax / 2,
		ConvertVector2ToVector3(start),
		ConvertVector2ToVector3(end)
	);

	start = 
		text_->GetPosition();

	end = {text_->GetPosition().x,WinApp::Win_Height/2};
	
	easeTextPosition_.Reset(
		Ease::In_,
		timeMax,
		ConvertVector2ToVector3(start),
		ConvertVector2ToVector3(end)
	);
}

void GameClearScene::Update() {
	input_->Update();

	if (isIntro_) {
		Introduction();
	}
	else {
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
			if (blackOut_->IsEnd()) {
				//シーンの切り替えを依頼
				SceneManager::GetInstance()->ChangeScene("TITLE");
			}
		}
		//else if (buttonRetry_->ChackClick(input_->ReleaseMouse(0))) {
		//	//シーンの切り替えを依頼
		//	SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
		//}
	}

	Vector3 playerWorldPos = {
		playerObj_->GetMatWorld().m[3][0],
		playerObj_->GetMatWorld().m[3][1],
		playerObj_->GetMatWorld().m[3][2]
	};

	const float distance = 100.0f;
	Vector3 posPlayer = playerWorldPos;
	Vector3 posCamera{};

	if (isIntro_) {
		posCamera = posPlayer - Vector3({ 0,0,distance });
	}
	else {
		CameraEase();
		posCamera = easeCameraPosition_.GetReturn();
		UIEase();
	}

	light_->Update();

	skydomeObj_->Update();

	doorL_->Update();
	doorR_->Update();

	PlayerUpdate();

	camera_->SetEye(posCamera);
	camera_->SetTarget(posPlayer);
	camera_->Update();

	//buttonRetry_->Update();
	buttonTitle_->Update();

	sprite_->Update();

	BlackOutUpdate();
}

void GameClearScene::Draw() {
#ifdef _DEBUG
	imGuiManager_->Begin();

	imGuiManager_->End();
#endif

	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());

	doorL_->Draw();
	doorR_->Draw();
	skydomeObj_->Draw();
	playerObj_->Draw();
	cart_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	SpriteBasis::GetInstance()->PreDraw();

	//sprite_->Draw();
	if (isIntro_) {
	}
	else {
		//buttonRetry_->Draw();
		buttonTitle_->Draw();
		text_->Print();
	}

	text_->DrawAll();

	blackOut_->Draw();
	arrangeTile_->Draw();

	SpriteBasis::GetInstance()->PostDraw();
}

void GameClearScene::Finalize() {
	SafeDelete(cart_);
	SafeDelete(cartModel_);

	SafeDelete(playerObj_);
	SafeDelete(skydomeObj_);
	SafeDelete(playerModel_);
	SafeDelete(skydomeModel_);
	SafeDelete(doorModel_);

	SafeDelete(sprite_);

	SafeDelete(light_);
	SafeDelete(camera_);

	buttonRetry_->Finalize();
	SafeDelete(buttonRetry_);

	buttonTitle_->Finalize();
	SafeDelete(buttonTitle_);

	SafeDelete(text_);

	SafeDelete(doorL_);
	SafeDelete(doorR_);

	blackOut_->Finalize();
	SafeDelete(blackOut_);
	SafeDelete(arrangeTile_);
}

void GameClearScene::Introduction() {
	float rotaSpeed = ConvertToRadian(1.0f);
	float posSpeed = 1.0f;

	Vector3 rota{};
	Vector3 pos{};

	rota = doorL_->GetRotation();
	if (rota.y <= ConvertToRadian(120)) {

		rota += Vector3{ 0,rotaSpeed,0 };
		doorL_->SetRotation(rota);

		pos = doorL_->GetPosition();
		pos += Vector3{ -posSpeed,0,0 };
		doorL_->SetPosition(pos);
	}

	rota = doorR_->GetRotation();
	if (rota.y >= ConvertToRadian(-120)) {
		rota += Vector3{ 0,-rotaSpeed,0 };
		doorR_->SetRotation(rota);

		pos = doorR_->GetPosition();
		pos += Vector3{ posSpeed,0,0 };
		doorR_->SetPosition(pos);
	}

	Vector3 playerWorldPos = {
		playerObj_->GetMatWorld().m[3][0],
		playerObj_->GetMatWorld().m[3][1],
		playerObj_->GetMatWorld().m[3][2]
	};

	Vector3 posPlayer = playerWorldPos;

	if (playerWorldPos.z >= doorPos_.z + 10.0f) {
		float endPointZ = 900.0f;

		//イージング
		easeCameraPosition_.Reset(
			Ease::In_,
			60,
			camera_->GetEye(),
			{
				playerWorldPos.x + 20.0f,
				playerWorldPos.y + 20.0f,
				endPointZ + 20.0f
			}
		);

		playerObj_->SetRotation(CreateRotationVector(
			{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));

		isIntro_ = false;
	}
	else {
		posPlayer += Vector3{ 0,0,(float)cPlayerSpeed_ };

	}
	playerObj_->SetPosition(posPlayer);
}

float GameClearScene::RoopFloat(float f, float speed, float min, float max) {
	static float t = speed;

	if (f < min || f > max) {
		t *= -1.0f;
	}

	f += t;

	return f;
}

void GameClearScene::CameraEase() {
	easeCameraPosition_.Update();
}

void GameClearScene::UIEase() {
	Vector3 move{};

	if (easeTextPosition_.IsEnd()) {

	easeButtonPosition_.Update();
	move = easeButtonPosition_.GetReturn();

	buttonTitle_->SetPosition(
		ConvertVector3ToVector2(move)
	);
	}

	buttonTitle_->Update();

	easeTextPosition_.Update();
	move = easeTextPosition_.GetReturn();

	text_->SetPosition(
		ConvertVector3ToVector2(move)
	);
}

void GameClearScene::BlackOutUpdate() {
	if (isGoTitle_) {
		blackOut_->SetIs(true);
		blackOut_->SetIsOpen(false);
		if (blackOut_->IsEnd()) {
			//シーンの切り替えを依頼
			SceneManager::GetInstance()->ChangeScene("TITLE");
		}
	}
	else {
		goTitleTimer_--;
	}

	if (goTitleTimer_ <= 0) {
		isGoTitle_ = true;
		goTitleTimer_ = 0;
	}

	blackOut_->Update();

	if (arrangeTile_->IsOpen()) {
		arrangeTile_->Update();
	}

	if (arrangeTile_->IsEnd()) {
		arrangeTile_->SetIs(false);
	}
}

void GameClearScene::PlayerUpdate() {
	Vector3 pos = playerObj_->GetPosition();
	Vector3 move{ 0,0,0 };

	float endPointZ = 900.0f;

	if (pos.z < endPointZ) {
		move.z = (float)cPlayerSpeed_;
		pos += move;
	}

	playerObj_->SetPosition(pos);

	playerObj_->Update();

	cart_->SetPosition({
		playerObj_->GetPosition().x,
		playerObj_->GetPosition().y-15.0f,
		playerObj_->GetPosition().z}
	);

	cart_->Update();
}
