/*ゲームプレイシーン*/

#include "GamePlayScene.h"
#include "SafeDelete.h"
#include "Quaternion.h"
#include "CollisionManager.h"
#include "SceneManager.h"
#include "Random.h"
#include "Cursor.h"

#pragma region popLoader
#include <fstream>
#include <Framework.h>
#pragma endregion

#include <cassert>

#ifdef _DEBUG
#include <imgui.h>
#endif

DirectXBasis* GamePlayScene::dxBas_ = DirectXBasis::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
SpriteBasis* GamePlayScene::spriteBas_ = SpriteBasis::GetInstance();
CollisionManager* GamePlayScene::collisionManager_ = CollisionManager::GetInstance();

#ifdef _DEBUG
ImGuiManager* GamePlayScene::imGuiManager_ = ImGuiManager::GetInstance();
#endif

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
	planeModel_ = Model::LoadFromOBJ("human", true);
	planeEnemyModel_ = Model::LoadFromOBJ("planeEnemy", true);

	skydomeModel_ = new Model();
	skydomeModel_ = Model::LoadFromOBJ("skydome", false);

	bulletModel_ = new Model();
	bulletModel_ = Model::LoadFromOBJ("missile", true);

	tubeModel_ = new Model();
	tubeModel_ = Model::LoadFromOBJ("BG_Tube", true);

#pragma region Player
	player_ = Player::Create(planeModel_);
	player_->SetGameScene(this);
	player_->SetBulletModel(bulletModel_);
	player_->SetCamera(camera_);
	//player_->Update();

#pragma endregion

#pragma region Enemy
	LoadEnemyPopData("enemyPop");
#pragma endregion

#pragma region Skydome
	skydome_ = Skydome::Create();
	skydome_->SetModel(skydomeModel_);
	skydome_->SetScale({ 1024.0f, 256.0f, 1024.0f });
	skydome_->SetPosition({ 0,0,0 });
	skydome_->SetCamera(camera_);
#pragma endregion

#pragma region Tube
	tubeManager_ = new TubeManager();

	tubeManager_->SetSpeed(16.0f);

	tubeManager_->SetRotation(CreateRotationVector(
		{ 0.0f,0.0f,1.0f }, ConvertToRadian(180.0f)));

	tubeManager_->SetScale({ 100,100,100 });

	tubeManager_->SetCamera(camera_);

	tubeManager_->SetTubeModel(tubeModel_);

	tubeManager_->Initialize();

#pragma endregion

#pragma region 扉
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
#pragma endregion

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	Object3d::SetLight(light_);

	//パーティクル
	particle_ = Particle::LoadFromObjModel("particle.png");
	pm_ = ParticleManager::Create();
	pm_->SetParticleModel(particle_);
	pm_->SetCamera(camera_);
	pm_->SetColor({ 0.7f,0.4f,0.1f,0.7f });
}

void GamePlayScene::Initialize2d() {
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
}

void GamePlayScene::Update3d() {
	{
		static Vector3 lightDir = { 0,1,5 };
		light_->SetDirLightDir(0, lightDir);
	}
	if (player_->IsStart()) {

		//自機弾自壊フラグの立った弾を削除
		playerBullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
			return bullet->IsDead();
			});

		//敵機弾自壊フラグの立った弾を削除
		enemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
			return bullet->IsDead();
			});

		//敵機が全滅したら(コンテナが空になったら)
		if (enemys_.size() <= 0) {
			//ファイナルフェイズに届いてなければ
			if (phaseIndex_ < kFinalPhaseIndex_) {
				//次の敵の湧き位置検索
				isWait_ = false;
				//フェーズ番号を進める
				phaseIndex_++;
				//レールカメラを前進させる
				railCamera_->SetPhaseAdvance(true);
			}
			else {
				//blackOut_->SetIs(true);
				//blackOut_->SetIsOpen(false);
				//if (blackOut_->IsEnd()) {
				//	//シーンの切り替えを依頼
				//	SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
				//}

				//シーンの切り替えを依頼
				SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
			}
		}

		//敵機のダメージ処理
		for (std::unique_ptr<Enemy>& enemy : enemys_) {

			if (isGushing_) {
				pm_->Active(
					{
						enemy->GetMatWorld().m[3][0],
						enemy->GetMatWorld().m[3][1],
						enemy->GetMatWorld().m[3][2] },
					{2.0f,0.0f,2.0f },
					{-2.0f,0.0f,-2.0f },

					{ 3.0f,10.0f,3.0f },
					{ -3.0f,0.0f,-3.0f },
					{ 0.0f,0.001f,0.0f },
					1000,
					5.0f,
					0.0f,
					20
					);
			}

			isGushing_ = false;
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
		player_->SetWorldTransformRailCamera(railCamera_->GetWorldTransform());
		//敵機のダメージ処理
		for (std::unique_ptr<Enemy>& enemy : enemys_) {
			enemy->Update();
			//被ダメージ処理
			if (enemy->IsDamage()) {
				float life = enemy->GetLife();

				life -= nowDamagePlayer_;
				enemy->SetLife(life);

				enemy->SetIsDamage(false);

				pm_->Active(
					{
						enemy->GetMatWorld().m[3][0],
						enemy->GetMatWorld().m[3][1],
						enemy->GetMatWorld().m[3][2] },
					{ 2.0f ,2.0f,2.0f },
					{ 2.0f ,2.0f,2.0f },

					{ 5.0f,5.0f,5.0f },
					{ 5.0f,5.0f,5.0f },

					{ 0.0f,0.001f,0.0f },
					100,
					2.0f,
					0.0f,
					10
					);
			}
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

			pm_->Active(
				{
					player_->GetMatWorld().m[3][0],
					player_->GetMatWorld().m[3][1],
					player_->GetMatWorld().m[3][2] },
				{ 2.0f ,2.0f,2.0f },
				{ 2.0f ,2.0f,2.0f },

				{ 5.0f,5.0f,5.0f },
				{ 5.0f,5.0f,5.0f },

				{ 0.0f,0.001f,0.0f },
				100,
				3.0f,
				0.0f,
				10
				);
		}

		if (player_->IsOver()) {
			blackOut_->SetIs(true);
			blackOut_->SetIsOpen(false);
			if (blackOut_->IsEnd()) {
				//シーンの切り替えを依頼
				SceneManager::GetInstance()->ChangeScene("GAMEOVER");
			}
		}

		UpdateEnemyPopCommands();
	}
	else {
		Vector3 startCameraMove = railCamera_->GetWorldTransform()->position_;

		startCameraMove += {0.0f, 0.0f, -0.2f};

		railCamera_->GetWorldTransform()->position_ = startCameraMove;
	}
	camera_->SetEye(railCamera_->GetEye());
	camera_->SetTarget(railCamera_->GetTarget());
	camera_->SetUp(railCamera_->GetUp());
	player_->SetWorldTransformRailCamera(railCamera_->GetWorldTransform());
	camera_->Update();
	light_->Update();

	skydome_->Update();

	doorL_->Update();
	doorR_->Update();

	//マウス座標
	Vector2 mousePos = input_->GetMousePosition();

	if (player_->IsStart()) {
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
			enemy->Update();

			if (!cursor_.IsLockOn()) {
				enemyWorldPos_ = {
					enemy->GetMatWorld().m[3][0],
					enemy->GetMatWorld().m[3][1] - 1.0f,
					enemy->GetMatWorld().m[3][2]
				};

			}
			//自機と敵機の距離(仮)
			float distancePToE = 30.0f;

			//カーソルから3Dレティクルまでの距離を設定
			cursor_.SetDistance(70.0f + distancePToE);

			//マウスカーソルから、3D照準座標を取得する
			LockOnTargetPos_ =
				cursor_.Get3DReticlePosition(camera_, enemyWorldPos_);
		}

		//自機のレティクル更新
		player_->UpdateReticle(LockOnTargetPos_);
	}


#pragma region Tube
	tubeManager_->Update();
#pragma endregion

	player_->Update();

	pm_->Update();

	blackOut_->Update();

	BlackOutUpdate();
}

void GamePlayScene::Update2d() {
}

void GamePlayScene::Draw3d() {
	//天球描画
	skydome_->Draw();

#pragma region Tube
	tubeManager_->Draw();
#pragma endregion

	doorL_->Draw();
	doorR_->Draw();

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
	pm_->Draw();
}

void GamePlayScene::Draw2d() {
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->DrawUI();
	}
	player_->DrawUI();

	blackOut_->Draw();
	arrangeTile_->Draw();
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

void GamePlayScene::LoadEnemyPopData(std::string filename) {
	std::ifstream file;

	//ディレクトリパス
	std::string Directory = "Resource/csv/";
	//フォーマットを今回はcsvに
	std::string format = ".csv";
	//フルパスを得る
	std::string fullpath = Directory + filename + format;

	//フルパスでオープン
	file.open(fullpath);
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands_ << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

void GamePlayScene::UpdateEnemyPopCommands() {
	//待機処理
	if (isWait_) {
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	Vector3 position{};
	Vector3 rotation{};
	Vector3 scale{};
	float radian = 0.0f;

	//コマンド実行ループ
	while (getline(enemyPopCommands_, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;

		//区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		//POSITIONコマンド
		if (word.find("POSITION") == 0) {
			position = LoadCommandsVector3(
				&line_stream,
				word);
		}

		//ROTATIONコマンド
		if (word.find("ROTATION") == 0) {
			rotation = LoadCommandsVector3(
				&line_stream,
				word);

			getline(line_stream, word, ',');
			radian = (float)std::atof(word.c_str());
		}

		//SCALEコマンド
		if (word.find("SCALE") == 0) {
			scale = LoadCommandsVector3(
				&line_stream,
				word);
		}

		//POPコマンド
		if (word.find("POP") == 0) {
			//敵を発生させる
			AddEnemy(
				position,
				CreateRotationVector(
					rotation,
					ConvertToRadian(radian)
				),
				scale,
				Enemy::Gun_BulletType);
		}

		//WAITコマンド
		else if (word.find("WAIT") == 0) {
			//待機時間
			isWait_ = true;
			//ループ抜け
			break;
		}
	}
}

Vector3 GamePlayScene::LoadCommandsVector3(
	std::istringstream* line_stream,
	std::string word) {
	Vector3 result{};

	//x
	getline(*line_stream, word, ',');
	result.x = (float)std::atof(word.c_str());

	//y
	getline(*line_stream, word, ',');
	result.y = (float)std::atof(word.c_str());

	//z
	getline(*line_stream, word, ',');
	result.z = (float)std::atof(word.c_str());

	return result;
}

void GamePlayScene::Finalize() {
	SafeDelete(skydome_);

#pragma region Tube
	tubeManager_->Finalize();
	SafeDelete(tubeManager_);
	SafeDelete(tubeModel_);
#pragma endregion

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

	SafeDelete(particle_);
	SafeDelete(pm_);

	blackOut_->Finalize();
	SafeDelete(blackOut_);
	SafeDelete(arrangeTile_);

	SafeDelete(doorModel_);
	SafeDelete(doorL_);
	SafeDelete(doorR_);
}

void GamePlayScene::BlackOutUpdate() {
	if (arrangeTile_->IsOpen()) {
		arrangeTile_->Update();
	}

	if (arrangeTile_->IsEnd()) {
		arrangeTile_->SetIs(false);
	}
}
