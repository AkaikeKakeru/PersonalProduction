#include "GamePlayScene.h"
#include "SafeDelete.h"
#include "Quaternion.h"
#include <imgui.h>
#include "Player.h"
#include "Enemy.h"
#include "CollisionManager.h"

DirectXBasis* GamePlayScene::dxBas_ = DirectXBasis::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
DrawBasis* GamePlayScene::drawBas_ = DrawBasis::GetInstance();
ImGuiManager* GamePlayScene::imGuiManager_ = ImGuiManager::GetInstance();
CollisionManager* GamePlayScene::collisionManager_ = CollisionManager::GetInstance();

void GamePlayScene::Initialize() {
	Initialize2d();
	Initialize3d();
}

void GamePlayScene::Update() {
	input_->Update();


	Update3d();
	Update2d();
	
	collisionManager_->CheckAllCollisions();
}

void GamePlayScene::Draw() {
#ifdef _DEBUG
	imGuiManager_->Begin();
	railCamera_->DrawImGui();
	enemy_->DrawImgui();
	player_->DrawImgui();
	imGuiManager_->End();
#endif // DEBUG
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

	camera_->SetEye({ 0.0f,10.0f,-20.0f });
	camera_->SetTarget({ 0.0f,0.0f,0.0f });
	camera_->SetUp({ 0.0f,1.0f,0.0f });
	camera_->Update();

	railCamera_ = new RailCamera();
	railCamera_->Initialize(camera_->GetEye(), { 0,0,0 });
	railCamera_->SetTarget(camera_->GetTarget());
	railCamera_->SetUp(camera_->GetUp());
	railCamera_->Update();

	planeModel_ = new Model();
	planeModel_ = Model::LoadFromOBJ("plane", true);
	planeEnemyModel_ = Model::LoadFromOBJ("planeEnemy", true);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome", false);

	bulletModel_ = new Model();
	bulletModel_ = Model::LoadFromOBJ("missile", true);

#pragma region Player
	player_ = Player::Create(planeModel_);
	player_->SetBulletModel(bulletModel_);
	//player_->SetScale({ 1.0f, 1.0f, 1.0f });
	//player_->SetRotation(CreateRotationVector(
	//	{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	//player_->SetPosition({ 0.0f,-5.0f,30.0f });

	player_->SetCamera(camera_);
	player_->Update();
#pragma endregion

#pragma region Enemy
	enemy_ = Enemy::Create(planeEnemyModel_);
	enemy_->SetBulletModel(bulletModel_);
	//enemy_->SetScale({ 1.0f, 1.0f, 1.0f });
	//enemy_->SetRotation(CreateRotationVector(
	//	{ 0.0f,1.0f,0.0f }, ConvertToRadian(180.0f)));
	//enemy_->SetPosition({ 0.0f,0.0f,100.0f });

	enemy_->SetCamera(camera_);
	enemy_->Update();
#pragma endregion

#pragma region Skydome
	skydomeObj_ = new Object3d();
	skydomeObj_ = Object3d::Create();
	skydomeObj_->SetModel(skydomeModel_);
	skydomeObj_->SetScale({ 30, 30, 30 });
	skydomeObj_->SetCamera(camera_);
#pragma endregion

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	Object3d::SetLight(light_);
}

void GamePlayScene::Initialize2d() {
}

void GamePlayScene::Update3d() {
	{
		static Vector3 lightDir = { 0,1,5 };
		light_->SetDirLightDir(0, lightDir);
	}

	railCamera_->Update();

	camera_->SetEye(railCamera_->GetEye());
	camera_->SetTarget(railCamera_->GetTarget());
	camera_->SetUp(railCamera_->GetUp());

	camera_->Update();

	player_->SetWorldTransformRailCamera(railCamera_->GetWorldTransform());

	light_->Update();

	skydomeObj_->Update();
	player_->Update();
	enemy_->Update();
}

void GamePlayScene::Update2d() {
}

void GamePlayScene::Draw3d() {
	//skydomeObj_->Draw();
	enemy_->Draw();
	player_->Draw();
}

void GamePlayScene::Draw2d() {
	enemy_->DrawUI();
	player_->DrawUI();
}

Vector3 GamePlayScene::CreateRotationVector(Vector3 axisAngle, float angleRadian) {
	Quaternion rotation = MakeAxisAngle(axisAngle, ConvertToRadian(1.0f));
	Vector3 point = axisAngle * angleRadian;

	return RotateVector(point, rotation);
}

void GamePlayScene::Finalize() {
	SafeDelete(skydomeObj_);

	enemy_->Finalize();
	SafeDelete(enemy_);

	player_->Finalize();
	SafeDelete(player_);

	SafeDelete(light_);
	SafeDelete(railCamera_);
	SafeDelete(camera_);

	SafeDelete(bulletModel_);
	SafeDelete(planeEnemyModel_);
	SafeDelete(planeModel_);
	SafeDelete(skydomeModel_);
}
