/*ゲームクリアシーン*/

#include "GameClearScene.h"
#include "SafeDelete.h"

#include "Framework.h"
#include "SceneManager.h"
#include <Quaternion.h>

#include "ObjectManager.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

DirectXBasis* GameClearScene::dxBas_ = DirectXBasis::GetInstance();
Input* GameClearScene::input_ = Input::GetInstance();
SpriteBasis* GameClearScene::spriteBas_ = SpriteBasis::GetInstance();
ObjectManager* GameClearScene::objManager_ = ObjectManager::GetInstance();

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

	Object3d* newObj = new Object3d();

	newObj = Object3d::Create();
	newObj->SetCamera(camera_);
	newObj->SetModel(objManager_->GetModel(playerActiveModel_));
	newObj->SetPosition({ 0, -5.0f, 150.0f });
	newObj->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));

	camera_->SetTarget(newObj->GetPosition());
	playerObj_.reset(newObj);

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

	newObj = new Object3d();
	newObj = Object3d::Create();
	newObj->SetCamera(camera_);
	newObj->SetModel(objManager_->GetModel(skydomeModel_));
	newObj->SetScale({ 1100.0f, 256.0f, 1100.0f });
	newObj->SetPosition({ 0,0,0 });
	skydomeObj_.reset(newObj);

	newObj = Object3d::Create();
	newObj->SetCamera(camera_);
	newObj->SetModel(objManager_->GetModel(bottomBGModel_));
	newObj->SetScale({ 10.0f, 10.0f, 10.0f });
	newObj->SetPosition({ 0,-100,0 });
	newObj->Update();
	bottomBG_.reset(newObj);

#pragma region Tube
	tubeManager_ = new TubeManager();
	tubeManager_->SetCamera(camera_);
	tubeManager_->SetSpeed(16.0f);
	tubeManager_->SetRotation(CreateRotationVector(
		{ 0.0f,0.0f,1.0f }, ConvertToRadian(180.0f)));
	tubeManager_->SetScale({ 100,100,100 });
	tubeManager_->SetTubeModel(objManager_->GetModel(tubeModel_));
	tubeManager_->Initialize();
#pragma endregion

	doorPos_ = { 0,0,800 };

	newObj = Object3d::Create();
	newObj->SetCamera(camera_);
	newObj->SetScale({ 50,400,10 });
	Vector3 scaDoorL = newObj->GetScale();

	newObj->SetPosition({ -scaDoorL.x, scaDoorL.y / 2,doorPos_.z });
	newObj->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	newObj->SetModel(objManager_->GetModel(doorModel_));
	doorL_.reset(newObj);

	newObj = Object3d::Create();
	newObj->SetCamera(camera_);
	newObj->SetScale({ 50,400,10 });
	Vector3 scaDoorR = newObj->GetScale();

	newObj->SetPosition({ scaDoorR.x, scaDoorR.y / 2, doorPos_.z });
	newObj->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	newObj->SetModel(objManager_->GetModel(doorModel_));
	doorR_.reset(newObj);

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
	newText->SetString("GAME CLEAR!");
	newText->SetPosition({
		WinApp::Win_Width / 2,
		-20.0f,
		});
	newText->SetSize({ textSize,textSize });
	text_.reset(newText);

	//ボタン
	textSize = 2.5f;
	Button* newButton = new Button();

	newButton->Initialize(0);
	newButton->SetTelop("Title");
	newButton->SetPosition({ -200.0f ,500.0f });
	newButton->SetSize({ 400.0f,96.0f });
	newButton->GetText()->SetSize({ textSize,textSize });
	newButton->Update();
	buttonTitle_.reset(newButton);

	Button* newButton2 = new Button();
	newButton2->Initialize(0);
	newButton2->SetTelop("Retry");
	newButton2->SetPosition({ WinApp::Win_Width - 300.0f ,500.0f });
	newButton2->SetSize({ 400.0f,96.0f });
	newButton2->GetText()->SetSize({ textSize,textSize });
	newButton2->Update();
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

	const int timeMax = 60;
	Vector2 start =
		buttonTitle_->GetPosition();
	Vector2 end = { 300.0f,buttonTitle_->GetPosition().y };

	easeButtonPosition_.Reset(
		Ease::In_,
		timeMax / 2,
		ConvertVector2ToVector3(start),
		ConvertVector2ToVector3(end)
	);

	start =
		text_->GetPosition();

	end = { text_->GetPosition().x,WinApp::Win_Height / 2 };

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
		tubeManager_->SetIsStop(true);

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
	bottomBG_->Update();

#pragma region Tube
	tubeManager_->Update();
#pragma endregion

	doorL_->Update();
	doorR_->Update();

	PlayerUpdate();

	camera_->SetEye(posCamera);
	camera_->SetTarget(posPlayer);
	camera_->Update();

	buttonTitle_->Update();

	BlackOutUpdate();
}

void GameClearScene::Draw() {
#ifdef _DEBUG
	imGuiManager_->Begin();

	imGuiManager_->End();
#endif

	//モデル本命処理
	Object3d::PreDraw(dxBas_->GetCommandList().Get());
	skydomeObj_->Draw();
	bottomBG_->Draw();
	if (isIntro_) {

		doorL_->Draw();
		doorR_->Draw();

#pragma region Tube
		tubeManager_->Draw();
#pragma endregion
	}

	playerObj_->Draw();
	cart_->Draw();

	Object3d::PostDraw();

	//スプライト本命処理
	SpriteBasis::GetInstance()->PreDraw();
;
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
#pragma region Tube
	tubeManager_->Finalize();
	SafeDelete(tubeManager_);
#pragma endregion

	SafeDelete(light_);
	SafeDelete(camera_);

	buttonRetry_->Finalize();
	buttonTitle_->Finalize();
	blackOut_->Finalize();
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
		playerObj_->GetPosition().y - 15.0f,
		playerObj_->GetPosition().z }
	);

	cart_->Update();
}
