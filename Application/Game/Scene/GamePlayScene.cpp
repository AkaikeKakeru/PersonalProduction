#include "GamePlayScene.h"
#include "SafeDelete.h"
#include "Quaternion.h"

DirectXBasis* GamePlayScene::dxBas_ = DirectXBasis::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
DrawBasis* GamePlayScene::drawBas_ = DrawBasis::GetInstance();

void GamePlayScene::Initialize() {
	Initialize2d();
	Initialize3d();
}

void GamePlayScene::Update() {
	input_->Update();

	Update3d();
	Update2d();
}

void GamePlayScene::Draw() {
#ifdef _DEBUG

#endif

	Object3d::PreDraw(dxBas_->GetCommandList().Get());
	Draw3d();
	Object3d::PostDraw();

	drawBas_->PreDraw();
	Draw2d();
	drawBas_->PostDraw();
}

void GamePlayScene::Initialize3d() {
	//カメラ生成
	camera_ = new Camera();
	camera_player = new Camera();

	planeModel_ = new Model();
	planeModel_ = Model::LoadFromOBJ("plane", true);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome", false);

	planeObj_ = new Object3d();
	planeObj_ = Object3d::Create();
	planeObj_->SetModel(planeModel_);
	planeObj_->SetScale({ 1, 1, 1 });
	planeObj_->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(180.0f)));
	planeObj_->SetCamera(camera_player);

	skydomeObj_ = new Object3d();
	skydomeObj_ = Object3d::Create();
	skydomeObj_->SetModel(skydomeModel_);
	skydomeObj_->SetScale({ 30, 30, 30 });
	skydomeObj_->SetCamera(camera_);

	//ライト生成
	light_ = new Light();
	light_ = Light::Create();
	light_->SetLightColor({ 1,1,1 });
	Object3d::SetLight(light_);
}

void GamePlayScene::Initialize2d() {
	drawBas_->LoadTexture(1, "texture.png");
	sprite_->Initialize(drawBas_, 1);

	sprite_->SetAnchorPoint({ 0.5f, 0.5f });
	sprite_->SetSize({ 64,64 });
}

void GamePlayScene::Update3d() {
	// オブジェクト回転
	{
		// 現在の座標を取得
		Vector3 rot = skydomeObj_->GetRotation();

		// 移動後の座標を計算

		//回転軸アングル
		//ここではY軸回転を指定
		Quaternion rotation = MakeAxisAngle(
			{ 0.0f,1.0f,0.0f }, ConvertToRadian(1.0f));

		//1F当たりの回転角度を指定
		Vector3 pointY = { 0.0f,ConvertToRadian(1.0f),0.0f };

		rot += RotateVector(pointY, rotation);

		// 座標の変更を反映
		skydomeObj_->SetRotation(rot);
	}

	// オブジェクト移動
	if (input_->PressKey(DIK_UP) ||
		input_->PressKey(DIK_DOWN) ||
		input_->PressKey(DIK_RIGHT) ||
		input_->PressKey(DIK_LEFT))
	{
		// 現在の座標を取得
		Vector3 position = planeObj_->GetPosition();
		// 現在の座標を取得
		Vector3 rot = planeObj_->GetRotation();

		//回転ベクトル
		Vector3 rotVector = {};

		//移動スピード
		float moveSpeed = 0.5f;
		//回転スピード
		float rotSpeed = ConvertToRadian(1.0f);

		Vector3 angleX = { 1.0f,0.0f,0.0f };
		Vector3 angleZ = { 0.0f,0.0f,1.0f };

		//移動後の座標を計算
		if (input_->PressKey(DIK_UP)) {
			// 移動後の座標を計算
			position.y += moveSpeed;

			rotVector = CreateRotationVector(
				angleX, rotSpeed);
		}

		else if (input_->PressKey(DIK_DOWN)) {
			position.y -= moveSpeed;

			rotVector = CreateRotationVector(
				angleX, -rotSpeed);
		}

		if (input_->PressKey(DIK_RIGHT)) {
			position.x += moveSpeed;

			rotVector = CreateRotationVector(
				angleZ, rotSpeed);
		}

		else if (input_->PressKey(DIK_LEFT)) {
			position.x -= moveSpeed;

			rotVector = CreateRotationVector(
				angleZ, -rotSpeed);
		}

		rot += rotVector;

		// 座標の変更を反映
		planeObj_->SetRotation(rot);

		// 座標の変更を反映
		planeObj_->SetPosition(position);
	}

	{
		static Vector3 lightDir = { 0,1,5 };

		//	if (input_->PressKey(DIK_W) ||
		//		input_->PressKey(DIK_S) ||
		//		input_->PressKey(DIK_D) ||
		//		input_->PressKey(DIK_A)) {
		//		if (input_->PressKey(DIK_W)) { lightDir.y += 1.0f; }
		//		else if (input_->PressKey(DIK_S)) { lightDir.y -= 1.0f; }
		//		if (input_->PressKey(DIK_D)) { lightDir.x += 1.0f; }
		//		else if (input_->PressKey(DIK_A)) { lightDir.x -= 1.0f; }
		//	}
		light_->SetLightDir(lightDir);
	}

	// カメラ移動
	if (input_->PressKey(DIK_W) ||
		input_->PressKey(DIK_S) ||
		input_->PressKey(DIK_D) ||
		input_->PressKey(DIK_A)) {
		if (input_->PressKey(DIK_W)) { camera_->MoveVector({ 0.0f,+1.0f,0.0f }); }
		else if (input_->PressKey(DIK_S)) { camera_->MoveVector({ 0.0f,-1.0f,0.0f }); }
		if (input_->PressKey(DIK_D)) { camera_->MoveVector({ +1.0f,0.0f,0.0f }); }
		else if (input_->PressKey(DIK_A)) { camera_->MoveVector({ -1.0f,0.0f,0.0f }); }
	}

	camera_->Update();
	camera_player->Update();
	light_->Update();
	skydomeObj_->Update();
	planeObj_->Update();
}

void GamePlayScene::Update2d() {
	// 現在の座標を取得
	Vector2 position = input_->GetMousePosition();

	//移動後の座標を計算

	// 座標の変更を反映
	sprite_->SetPosition(position);

	sprite_->Update();
}

void GamePlayScene::Draw3d() {
	skydomeObj_->Draw();
	planeObj_->Draw();
}

void GamePlayScene::Draw2d() {
	sprite_->Draw();
}

Vector3 GamePlayScene::CreateRotationVector(Vector3 axisAngle, float angleRadian) {
	Quaternion rotation = MakeAxisAngle(axisAngle, ConvertToRadian(1.0f));
	Vector3 point = axisAngle * angleRadian;

	return RotateVector(point, rotation);
}

void GamePlayScene::Finalize() {
	SafeDelete(planeObj_);
	SafeDelete(skydomeObj_);
	SafeDelete(planeModel_);
	SafeDelete(skydomeModel_);
	SafeDelete(sprite_);

	SafeDelete(light_);
	SafeDelete(camera_);
	SafeDelete(camera_player);
}
