#include "GamePlayScene.h"
#include "SafeDelete.h"
#include "Quaternion.h"
#include <imgui.h>
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
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->DrawImgui();
	}
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
	railCamera_->SetGameScene(this);
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
	AddEnemy({ -70.0f,0.0f,30.0f },
		CreateRotationVector(
			{ 0.0f,1.0f,0.0f }, ConvertToRadian(90.0f)),
		{ 1.0f, 1.0f, 1.0f });

	AddEnemy({ -70.0f,0.0f,60.0f },
		CreateRotationVector(
			{ 0.0f,1.0f,0.0f }, ConvertToRadian(90.0f)),
		{ 1.0f,1.0f,1.0f });

	AddEnemy({ -70.0f,10.0f,45.0f },
		CreateRotationVector(
			{ 0.0f,1.0f,0.0f }, ConvertToRadian(90.0f)),
		{ 1.0f,1.0f,1.0f });

	{
	//std::unique_ptr<Enemy> newEnemy =
	//	std::make_unique<Enemy>();
	//newEnemy->Initialize();

	//newEnemy->SetScale({ 1.0f, 1.0f, 1.0f });
	//newEnemy->SetRotation(CreateRotationVector(
	//	{ 0.0f,1.0f,0.0f }, ConvertToRadian(180.0f)));
	//newEnemy->SetPosition({ -70.0f,0.0f,30.0f });

	//newEnemy->SetModel(planeEnemyModel_);
	//newEnemy->SetCamera(camera_);
	//newEnemy->Update();
	////リストに登録
	//enemys_.push_back(std::move(newEnemy));
	}
	{
	//enemy_ = Enemy::Create(planeEnemyModel_);
	//enemy_->SetGameScene(this);
	//enemy_->SetBulletModel(bulletModel_);
	//enemy_->SetScale({ 1.0f, 1.0f, 1.0f });
	//enemy_->SetRotation(CreateRotationVector(
	//	{ 0.0f,1.0f,0.0f }, ConvertToRadian(180.0f)));
	//enemy_->SetPosition({ 0.0f,0.0f,100.0f });

	//enemy_->SetCamera(camera_);
	//enemy_->Update();
	}
#pragma endregion

#pragma region Skydome
	skydomeObj_ = new Object3d();
	skydomeObj_ = Object3d::Create();
	skydomeObj_->SetModel(skydomeModel_);
	skydomeObj_->SetScale({ 200, 200, 200 });
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

	//自壊フラグの立った弾を削除
	enemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->IsDead();
		});

	//for (std::unique_ptr<Enemy>& enemy : enemys_) {
	//	if(enemy->IsDead()){
	//		//railCamera_->SetPhaseAdvance(true);
	//		//AddEnemy({ 70.0f,0.0f,80.0f },
	//		//	CreateRotationVector(
	//		//		{ 0.0f,1.0f,0.0f }, ConvertToRadian(180.0f)),
	//		//	{ 1.0f,1.0f,1.0f });

	//	}
	//}

	if (enemys_.size() == 0) {
		phaseIndex_++;
		railCamera_->SetPhaseAdvance(true);
		AddEnemy({ 70.0f,0.0f,80.0f },
			CreateRotationVector(
				{ 0.0f,1.0f,0.0f }, ConvertToRadian(-90.0f)),
			{ 1.0f,1.0f,1.0f });

		AddEnemy({ 70.0f,-10.0f,60.0f },
			CreateRotationVector(
				{ 0.0f,1.0f,0.0f }, ConvertToRadian(-90.0f)),
			{ 1.0f,1.0f,1.0f });
	}

	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return enemy->IsDead();
		});

	railCamera_->Update();

	camera_->SetEye(railCamera_->GetEye());
	camera_->SetTarget(railCamera_->GetTarget());
	camera_->SetUp(railCamera_->GetUp());

	camera_->Update();

	player_->SetWorldTransformRailCamera(railCamera_->GetWorldTransform());

	light_->Update();

	skydomeObj_->Update();
	player_->Update();
	//弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Update();
	}

	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Update();
	}
}

void GamePlayScene::Update2d() {
}

void GamePlayScene::Draw3d() {
	skydomeObj_->Draw();

	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Draw();
	}
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Draw();
	}

	player_->Draw();
}

void GamePlayScene::Draw2d() {
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->DrawUI();
	}
	player_->DrawUI();
}

Vector3 GamePlayScene::CreateRotationVector(Vector3 axisAngle, float angleRadian) {
	Quaternion rotation = MakeAxisAngle(axisAngle, ConvertToRadian(1.0f));
	Vector3 point = axisAngle * angleRadian;

	return RotateVector(point, rotation);
}

void GamePlayScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet) {
	//リストに登録
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GamePlayScene::AddEnemy(
	const Vector3 pos,
	const Vector3 rota,
	const Vector3 scale) {
	std::unique_ptr<Enemy> newEnemy =
		std::make_unique<Enemy>();
	newEnemy->Initialize();
	newEnemy->SetGameScene(this);

	newEnemy->SetScale(scale);
	newEnemy->SetRotation(rota);
	newEnemy->SetPosition(pos);

	newEnemy->SetModel(planeEnemyModel_);
	newEnemy->SetBulletModel(bulletModel_);

	newEnemy->SetCamera(camera_);
	newEnemy->Update();
	//リストに登録
	enemys_.push_back(std::move(newEnemy));
}

void GamePlayScene::Finalize() {
	SafeDelete(skydomeObj_);


	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Finalize();
	}
	//SafeDelete(enemy_);

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
