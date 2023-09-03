#include "GamePlayScene.h"
#include "SafeDelete.h"
#include "Quaternion.h"
#include <imgui.h>
#include "CollisionManager.h"
#include "SceneManager.h"
#include "Random.h"

DirectXBasis* GamePlayScene::dxBas_ = DirectXBasis::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
SpriteBasis* GamePlayScene::spriteBas_ = SpriteBasis::GetInstance();
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

	ParticleManager::PreDraw(dxBas_->GetCommandList().Get());
	DrawParticle();
	ParticleManager::PostDraw();

	SpriteBasis::GetInstance()->PreDraw();
	Draw2d();
	SpriteBasis::GetInstance()->PostDraw();
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

	debugCamera_ = new DebugCamera();
	debugCamera_->Initialize({ 0,20,-50 }, { ConvertToRadian(10),0,0 });

	planeModel_ = new Model();
	planeModel_ = Model::LoadFromOBJ("plane", true);
	planeEnemyModel_ = Model::LoadFromOBJ("planeEnemy", true);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome", false);

	bulletModel_ = new Model();
	bulletModel_ = Model::LoadFromOBJ("missile", true);

#pragma region Player
	player_ = Player::Create(planeModel_);
	player_->SetGameScene(this);
	player_->SetBulletModel(bulletModel_);
	player_->SetCamera(camera_);
	player_->Update();
#pragma endregion

#pragma region Enemy
	AddEnemy({ 0.0f,-1024.0f,-1024.0f },
		CreateRotationVector(
			{ 0.0f,1.0f,0.0f }, ConvertToRadian(-90.0f)),
		{ 1.0f,1.0f,1.0f },
		Enemy::Axe_BulletType);
#pragma endregion

#pragma region Skydome
	skydome_ = Skydome::Create();
	skydome_->SetModel(skydomeModel_);
	skydome_->SetScale({ 512.0f, 126.0f, 512.0f });
	skydome_->SetPosition({ 0,0,0 });
	skydome_->SetCamera(camera_);
	skydome_->Update();

	skydome2_ = Skydome::Create();
	skydome2_->SetModel(skydomeModel_);
	skydome2_->SetScale({ 512.0f, 126.0f, 512.0f });
	skydome2_->SetPosition({ 0,0,skydome2_->GetScale().z});
	skydome2_->SetCamera(camera_);
	skydome2_->Update();
#pragma endregion

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	Object3d::SetLight(light_);

	//パーティクル
	//particle_ = Particle::LoadFromObjModel("particle2.png");
	//pm_ = ParticleManager::Create();
	//pm_->SetParticleModel(particle_);
	//pm_->SetCamera(camera_);
}

void GamePlayScene::Initialize2d() {
}

void GamePlayScene::Update3d() {
	{
		static Vector3 lightDir = { 0,1,5 };
		light_->SetDirLightDir(0, lightDir);
	}

	//自機弾自壊フラグの立った弾を削除
	playerBullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});

	//敵機弾自壊フラグの立った弾を削除
	enemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->IsDead();
		});

	//敵機が全滅したら(コンテナが空になったら)
	if (enemys_.size() <= 1) {
		//ファイナルフェイズに届いてなければ
		if (phaseIndex_ < kFinalPhaseIndex_) {
			//次の敵の湧き位置検索
			SightNextEnemy();
			//フェーズ番号を進める
			phaseIndex_++;
			//レールカメラを前進させる
			railCamera_->SetPhaseAdvance(true);
		}
		else {
			//シーンの切り替えを依頼
			SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
		}
	}

	//敵機をデスフラグで削除
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return enemy->IsDead();
		});

	railCamera_->Update();

#ifdef _DEBUG
	if (input_->ReleaseKey(DIK_Q)) {
		isDebugCamera_ = !isDebugCamera_;
	}
#endif // _DEBUG

	//デバッグカメラが起動中なら
	if (isDebugCamera_) {
		debugCamera_->Update();

		railCamera_->SetEye(debugCamera_->GetEye());
		railCamera_->SetTarget(debugCamera_->GetTarget());
		railCamera_->SetUp(debugCamera_->GetUp());
	}

	camera_->SetEye(railCamera_->GetEye());
	camera_->SetTarget(railCamera_->GetTarget());
	camera_->SetUp(railCamera_->GetUp());

	camera_->Update();

	player_->SetWorldTransformRailCamera(railCamera_->GetWorldTransform());

	light_->Update();

	skydome_->Update();
	skydome2_->Update();
	
	player_->Update();

	//自機弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullets_) {
		bullet->Update();
	}

	//敵機弾更新
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Update();
	}

	//敵機の更新
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		

	//被ダメージ処理
		if (enemy->IsDamage()) {
			float life = enemy->GetLife();

			life -= nowDamagePlayer_;
			enemy->SetLife(life);

			enemy->SetIsDamage(false);
		}
		enemy->Update();

	}

	//自機の被ダメージ処理
	if (player_->IsDamage()) {
		bool hit = false;

		if (nowBulletTypeEnemy_ != EnemyBullet::Axe_BulletType) {
			if (player_->IsHide()) {
				hit = false;
			}
			else {
				hit = true;
			}
		}
		else {
			hit = true;
		}

		if (hit) {
			float life = player_->GetLife();
			life -= nowDamageEnemy_;
			player_->SetLife(life);
		}

		//ダメージ受けたらHPの変動を実行
		player_->GetHPGauge()->
			SetRest(player_->GetLife());

		player_->GetHPGauge()->
			DecisionFluctuation();
		player_->GetHPGauge()->
			SetIsFluct(true);

		player_->SetIsDamage(false);

		//pm_->Active(
		//	particle_, 
		//	{	player_->GetMatWorld().m[3][0],
		//		player_->GetMatWorld().m[3][1],
		//		player_->GetMatWorld().m[3][2]},
		//	{ 2.0f ,2.0f,2.0f },
		//	{ 5.0f,5.0f,5.0f },
		//	{ 0.0f,0.001f,0.0f },
		//	20,
		//	3.0f,
		//	0.0f,
		//	10
		//);
	}

	if (player_->IsDead()) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMEOVER");
	}

	//pm_->Update();
}

void GamePlayScene::Update2d() {
}

void GamePlayScene::Draw3d() {
	//天球描画
	skydome_->Draw();
	skydome2_->Draw();

	//敵機描画
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Draw();
	}

	//自機弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullets_) {
		bullet->Draw();
	}

	//敵機弾描画
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Draw();
	}

	//自機描画
	player_->Draw();
}

void GamePlayScene::DrawParticle() {
	//pm_->Draw();
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

void GamePlayScene::AddPlayerBullet(std::unique_ptr<PlayerBullet> playerBullet) {
	//リストに登録
	playerBullets_.push_back(std::move(playerBullet));
}

void GamePlayScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet) {
	//リストに登録
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GamePlayScene::AddEnemy(
	const Vector3 pos,
	const Vector3 rota,
	const Vector3 scale,
	const int bulletType) {
	std::unique_ptr<Enemy> newEnemy =
		std::make_unique<Enemy>();
	newEnemy->Initialize();
	newEnemy->SetGameScene(this);
	newEnemy->SetPlayer(player_);

	newEnemy->SetScale(scale);
	newEnemy->SetRotation(rota);
	newEnemy->SetPosition(pos);

	newEnemy->SetModel(planeEnemyModel_);
	newEnemy->SetBulletModel(bulletModel_);

	newEnemy->SetBulletType(bulletType);

	newEnemy->SetFireTimer(
		static_cast<int32_t>(
			RandomOutput(3.0f, 7.0f)
			)
	);

	newEnemy->SetCamera(camera_);
	newEnemy->Update();
	//リストに登録
	enemys_.push_back(std::move(newEnemy));
}

void GamePlayScene::SightNextEnemy() {
	switch (phaseIndex_) {
	case 0:
		//AddEnemy({ -70.0f,0.0f,30.0f },
		//	CreateRotationVector(
		//		{ 0.0f,1.0f,0.0f }, ConvertToRadian(90.0f)),
		//	{ 1.0f, 1.0f, 1.0f },
		//	Enemy::Gun_BulletType);

		AddEnemy({ -70.0f,0.0f,60.0f },
			CreateRotationVector(
				{ 0.0f,1.0f,0.0f }, ConvertToRadian(90.0f)),
			{ 1.0f,1.0f,1.0f },
			Enemy::Axe_BulletType);

		AddEnemy({ -70.0f,10.0f,45.0f },
			CreateRotationVector(
				{ 0.0f,1.0f,0.0f }, ConvertToRadian(90.0f)),
			{ 1.0f,1.0f,1.0f },
			Enemy::Gun_BulletType);
		break;

	case 1:
		//AddEnemy({ 70.0f,10.0f,90.0f },
		//	CreateRotationVector(
		//		{ 0.0f,1.0f,0.0f }, ConvertToRadian(-90.0f)),
		//	{ 1.0f,1.0f,1.0f },
		//	Enemy::Gun_BulletType);

		AddEnemy({ 70.0f,-10.0f,110.0f },
			CreateRotationVector(
				{ 0.0f,1.0f,0.0f }, ConvertToRadian(-90.0f)),
			{ 1.0f,1.0f,1.0f },
			Enemy::Axe_BulletType);
		break;

	case 2:
		AddEnemy({ 0.0f,10.0f,270.0f },
			CreateRotationVector(
				{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)),
			{ 1.0f,1.0f,1.0f },
			Enemy::Gun_BulletType);
		break;

	default:
		break;
	}
}

void GamePlayScene::Finalize() {
	SafeDelete(skydome_);
	SafeDelete(skydome2_);
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Finalize();
	}

	player_->Finalize();
	SafeDelete(player_);

	SafeDelete(light_);
	SafeDelete(debugCamera_);
	railCamera_->Finalize();
	SafeDelete(railCamera_);
	SafeDelete(camera_);

	SafeDelete(bulletModel_);
	SafeDelete(planeEnemyModel_);
	SafeDelete(planeModel_);
	SafeDelete(skydomeModel_);

	//SafeDelete(particle_);
	//pm_->Finalize();
}
