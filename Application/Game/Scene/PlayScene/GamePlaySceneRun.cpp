#include "GamePlaySceneRun.h"


#pragma region popLoader
#include <fstream>
#include <Framework.h>
#pragma endregion

#include <cassert>
#include <Quaternion.h>
#include <Random.h>

Input* GamePlaySceneRun::input_ = Input::GetInstance();
ObjectManager* GamePlaySceneRun::objManager_ = ObjectManager::GetInstance();

void GamePlaySceneRun::UpdateCamera() {

	followCamera_->Update();

	//デバッグカメラが起動中なら
	if (isDebugCamera_) {
		debugCamera_->Update();

		camera_->SetEye(debugCamera_->GetEye());
		camera_->SetTarget(debugCamera_->GetTarget());
		camera_->SetUp(debugCamera_->GetUp());
	}
	else {
		camera_->SetEye(followCamera_->GetEye());
		camera_->SetTarget(followCamera_->GetTarget());
		camera_->SetUp(followCamera_->GetUp());
	}

	camera_->Update();
}

void GamePlaySceneRun::UpdateCharacter() {
		//敵機の更新
		for (std::unique_ptr<Enemy>& enemy : enemys_) {
			enemy->Update();

			player_->UpdateReticle(enemy.get());

			if (!cursor_.IsLockOn()) {
				enemyWorldPos_ = {
					enemy->GetMatWorld().m[3][0],
					enemy->GetMatWorld().m[3][1],
					enemy->GetMatWorld().m[3][2]
				};

			}

			if (!player_->IsDead()) {
				//自機と敵機の距離(仮)
				float distancePToE =
					Vector3Dot(
						Vector3{
							enemy->GetMatWorld().m[3][0],
							enemy->GetMatWorld().m[3][1],
							enemy->GetMatWorld().m[3][2] },

							Vector3{
							player_->GetMatWorld().m[3][0],
							player_->GetMatWorld().m[3][1],
							player_->GetMatWorld().m[3][2] });

				//カーソルから3Dレティクルまでの距離を設定
				cursor_.SetDistance(distancePToE);

				//マウスカーソルから、3D照準座標を取得する
				LockOnTargetPos_ =
					cursor_.Get3DReticlePosition(camera_.get(), enemyWorldPos_);
			}
		}
	

	player_->Update();
}
void GamePlaySceneRun::UpdateDamage() {

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

			if (isGushing_) {
				pm_->Active(
					{
						enemy->GetMatWorld().m[3][0],
						enemy->GetMatWorld().m[3][1],
						enemy->GetMatWorld().m[3][2] },
					{ 2.0f,0.0f,2.0f },
					{ -2.0f,0.0f,-2.0f },

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
	
}
void GamePlaySceneRun::UpdateMouseUI() {
	static const Vector4 col = { 1,1,1,1 };
	static const Vector4 colPress = { 0.5f,0.2f,0.2f,1 };

	Vector4 colL, colR;

	if (input_->PressMouse(0)) {
		colL = colPress;
	}
	else {
		colL = col;
	}

	if (input_->PressMouse(1)) {
		colR = colPress;
	}
	else {
		colR = col;
	}

	mouseSprites_[mouseSpriteL_]->SetColor(colL);
	mouseSprites_[mouseSpriteR_]->SetColor(colR);
	for (std::unique_ptr<Sprite>& mouse : mouseSprites_) {
		mouse->Update();
	}
}

void GamePlaySceneRun::RemoveUniquePtr() {
	//敵機をデスフラグで削除
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return enemy->IsDead();
		});
}
void GamePlaySceneRun::DebugShortCut() {
#ifdef _DEBUG
		if (input_->TriggerKey(DIK_K)) {
			//敵機のダメージ処理
			for (std::unique_ptr<Enemy>& enemy : enemys_) {
				enemy->SetIsDead(true);
			}
		}
#endif // _DEBUG

#ifdef _DEBUG
	if (input_->ReleaseKey(DIK_Q)) {
		isDebugCamera_ = !isDebugCamera_;
	}
#endif // _DEBUG
}

void GamePlaySceneRun::PhaseChange() {
	//敵機が全滅したら(コンテナが空になったら)
	if (enemys_.size() <= 0) {
		//ファイナルフェイズに届いてなければ
		if (phaseIndex_ < kFinalPhaseIndex_) {
			//次の敵の湧き位置検索
			isWait_ = false;
			//フェーズ番号を進める
			phaseIndex_++;

			player_->SetPhaseAdvance(true);
		}
	}
}

void GamePlaySceneRun::InitializeCamera() {
	//カメラ生成
	camera_ = std::make_unique<Camera>();
	camera_->SetEye({ 0.0f,10.0f,-20.0f });
	camera_->SetTarget({ 0.0f,0.0f,0.0f });
	camera_->SetUp({ 0.0f,1.0f,0.0f });
	camera_->Update();

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(
		{ 0,20,-50 },
		{ ConvertToRadian(10),0,0 });

	//追従カメラの生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetEye({ 0.0f,10.0f,-20.0f });
	followCamera_->SetTarget({ 0.0f,0.0f,0.0f });
	followCamera_->SetUp({ 0.0f,1.0f,0.0f });
}
void GamePlaySceneRun::InitializeCharacter() {



#pragma region Player
	Player* newPlayer = Player::Create(
		objManager_->GetModel(playerActiveModel_));
	newPlayer->SetBulletModel(
		objManager_->GetModel(bulletModel_));
	newPlayer->SetModelPauses(
		objManager_->GetModel(playerActiveModel_),
		objManager_->GetModel(playerHideModel_));
	newPlayer->SetCartModel(objManager_->GetModel(cartModel_));

	newPlayer->SetCamera(camera_.get());
	//newPlayer->SetGamePlayScene(this);
	newPlayer->Initialize();
	player_.reset(newPlayer);
	player_->Update();
	//追従カメラの更新
	followCamera_->SetTargetWorldTransform(player_->GetWorldTransform());
	followCamera_->Update();
#pragma endregion

#pragma region Enemy
	LoadEnemyPopData("enemyPop");
#pragma endregion
}

void GamePlaySceneRun::Initialize() {
	InitializeCamera();
	InitializeCharacter();
}

void GamePlaySceneRun::Update() {
	Update3d();
	Update2d();
}

void GamePlaySceneRun::Draw() {
}

void GamePlaySceneRun::Finalize() {
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Finalize();
	}
	player_->Finalize();
}

void GamePlaySceneRun::Update3d() {
	RemoveUniquePtr();
		DebugShortCut();

		PhaseChange();

		UpdateCamera();

		UpdateDamage();

		UpdateEnemyPopCommands();


	UpdateCharacter();
	UpdateCamera();

	pm_->Update();
}

void GamePlaySceneRun::Update2d() {
	UpdateMouseUI();
}

void GamePlaySceneRun::Draw3d() {
	//敵機描画
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Draw();
	}

	//自機描画
	player_->Draw();
}

void GamePlaySceneRun::Draw2d() {
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->DrawUI();
	}
	player_->DrawUI();
}

void GamePlaySceneRun::DrawParticle() {
	pm_->Draw();
}

void GamePlaySceneRun::LoadEnemyPopData(std::string filename) {
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
void GamePlaySceneRun::UpdateEnemyPopCommands() {
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
Vector3 GamePlaySceneRun::LoadCommandsVector3(
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


void GamePlaySceneRun::AddEnemy(
	const Vector3 pos,
	const Vector3 rota,
	const Vector3 scale,
	const int bulletType) {
	std::unique_ptr<Enemy> newEnemy =
		std::make_unique<Enemy>();
	newEnemy->SetCamera(camera_.get());
	newEnemy->SetModel(objManager_->GetModel(enemyModel_));
	newEnemy->SetBulletModel(objManager_->GetModel(bulletModel_));
	newEnemy->SetCartModel(objManager_->GetModel(cartModel_));
	newEnemy->Initialize();
	//newEnemy->SetGamePlayScene(this);
	newEnemy->SetPlayer(player_.get());

	newEnemy->SetScale(scale);
	newEnemy->SetRotation(rota);
	newEnemy->SetPosition(pos);

	newEnemy->ReSetEasePos();

	newEnemy->SetBulletType(bulletType);

	newEnemy->SetFireTimer(
		static_cast<int32_t>(
			RandomOutput(3.0f, 7.0f)
			)
	);

	newEnemy->Update();
	//リストに登録
	enemys_.push_back(std::move(newEnemy));
}
