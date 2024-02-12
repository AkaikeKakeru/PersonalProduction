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

ObjectManager* GamePlayScene::objManager_ = ObjectManager::GetInstance();

#ifdef _DEBUG
ImGuiManager* GamePlayScene::imGuiManager_ = ImGuiManager::GetInstance();
#endif

void GamePlayScene::Finalize() {
	SafeDelete(light_);
	FinalizeBlackOut();
	FinalizeBackGround();
	FinalizeCharacter();
}

void GamePlayScene::FinalizeBackGround() {
#pragma region Tube
	tubeManager_->Finalize();
#pragma endregion

	skydome_->Finalize();
}
void GamePlayScene::FinalizeCharacter() {
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Finalize();
	}
	player_->Finalize();
}
void GamePlayScene::FinalizeBlackOut() {
	blackOut_->Finalize();
	arrangeTile_->Finalize();
}

void GamePlayScene::InitializeCamera() {
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
void GamePlayScene::InitializeBackground() {
#pragma region Skydome
	Skydome* newSkydome = Skydome::Create();
	skydome_.reset(newSkydome);
	skydome_->SetCamera(camera_.get());
	skydome_->SetModel(objManager_->GetModel(skydomeModel_));
	skydome_->SetScale({ 1024.0f, 256.0f, 1024.0f });
	skydome_->SetPosition({ 0,0,0 });
#pragma endregion

#pragma region Tube
	TubeManager* newTubeManager_ = new TubeManager();
	tubeManager_.reset(newTubeManager_);

	tubeManager_->SetCamera(camera_.get());
	tubeManager_->SetSpeed(16.0f);
	tubeManager_->SetRotation(CreateRotationVector(
		{ 0.0f,0.0f,1.0f }, ConvertToRadian(180.0f)));
	tubeManager_->SetScale({ 100,100,100 });
	tubeManager_->SetTubeModel(objManager_->GetModel(tubeModel_));
	tubeManager_->Initialize();
#pragma endregion

#pragma region 扉
	Object3d* obj = new Object3d();

	std::unique_ptr<Object3d> newObj =
		std::make_unique<Object3d>();

	doorPos_ = { 0,0,800 };

	obj = Object3d::Create();
	obj->SetCamera(camera_.get());
	obj->SetScale({ 50,400,10 });
	Vector3 scaDoorL = obj->GetScale();

	obj->SetPosition({ -scaDoorL.x, scaDoorL.y / 2,doorPos_.z });
	obj->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	obj->SetModel(objManager_->GetModel(doorModel_));

	newObj.reset(obj);
	objs_.push_back(std::move(newObj));

	obj = Object3d::Create();
	obj->SetCamera(camera_.get());
	obj->SetScale({ 50,400,10 });
	Vector3 scaDoorR = obj->GetScale();
	obj->SetPosition({ scaDoorR.x, scaDoorR.y / 2, doorPos_.z });
	obj->SetRotation(CreateRotationVector(
		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
	obj->SetModel(objManager_->GetModel(doorModel_));
	newObj.reset(obj);
	objs_.push_back(std::move(newObj));
#pragma endregion

	obj = Object3d::Create();
	obj->SetModel(objManager_->GetModel(bottomBGModel_));
	obj->SetScale({ 10.0f, 10.0f, 10.0f });
	obj->SetPosition({ 0,-100,0 });
	obj->SetCamera(camera_.get());
	obj->Update();

	newObj.reset(obj);
	objs_.push_back(std::move(newObj));
}
void GamePlayScene::InitializeCharacter() {

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
	newPlayer->SetGamePlayScene(this);
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

void GamePlayScene::InitializeMouseUI() {
	float size = 64.0f;
	float texSize = 64.0f;
	std::unique_ptr<Sprite> mouseL =
		std::make_unique<Sprite>();
	mouseL->Initialize(Framework::kMouseTextureIndex_);
	mouseL->SetPosition({
		WinApp::Win_Width - (texSize * 2),
		WinApp::Win_Height - (texSize * 2) });
	mouseL->SetSize({ size, size * 2 });
	mouseL->SetAnchorPoint({
		0.5f,
		0.5f });
	mouseL->SetTextureSize({
		texSize,
		(texSize * 2) });
	mouseL->SetTextureLeftTop({
		0,
		0 });
	//mouseSpriteL_.reset(newSprite);
	mouseSprites_.push_back(std::move(mouseL));

	std::unique_ptr<Sprite> mouseR =
		std::make_unique<Sprite>();
	mouseR->Initialize(Framework::kMouseTextureIndex_);
	mouseR->SetPosition({
		WinApp::Win_Width - texSize,
		WinApp::Win_Height - (texSize * 2) });
	mouseR->SetSize({ size, size * 2 });
	mouseR->SetAnchorPoint({
		0.5f,
		0.5f });
	mouseR->SetTextureSize({
		texSize,
		(texSize * 2) });
	mouseR->SetTextureLeftTop({
		texSize,
		0 });
	//mouseSpriteR_.reset(newSprite);
	mouseSprites_.push_back(std::move(mouseR));

	std::unique_ptr<Sprite> mouseW =
		std::make_unique<Sprite>();
	mouseW->Initialize(Framework::kMouseTextureIndex_);
	mouseW->SetPosition({
		WinApp::Win_Width - (texSize * 1.5f),
		WinApp::Win_Height - (texSize * 2) });
	mouseW->SetSize({ size, size });
	mouseW->SetAnchorPoint({
		0.5f,
		0.5f });
	mouseW->SetTextureSize({
		texSize,
		texSize });
	mouseW->SetTextureLeftTop({
		0,
		(texSize * 2) });
	//mouseSpriteWheel_.reset(newSprite);
	mouseSprites_.push_back(std::move(mouseW));

	std::unique_ptr<Sprite> mouseTS =
		std::make_unique<Sprite>();
	mouseTS->Initialize(Framework::kMouseTextureIndex_);
	mouseTS->SetPosition({
		WinApp::Win_Width - (texSize * 2),
		WinApp::Win_Height - (texSize * 1.5f) });
	mouseTS->SetSize({ size, size / 2 });
	mouseTS->SetAnchorPoint({
		0.5f,
		0.5f });
	mouseTS->SetTextureSize({
		texSize,
		texSize / 2 });
	mouseTS->SetTextureLeftTop({
		texSize,
		(texSize * 2) });
	//mouseSpriteTextS_.reset(newSprite);
	mouseSprites_.push_back(std::move(mouseTS));

	std::unique_ptr<Sprite> mouseTH =
		std::make_unique<Sprite>();
	mouseTH->Initialize(Framework::kMouseTextureIndex_);
	mouseTH->SetPosition({
		WinApp::Win_Width - texSize,
		WinApp::Win_Height - (texSize * 1.5f) });
	mouseTH->SetSize({ size, size / 2 });
	mouseTH->SetAnchorPoint({
		0.5f,
		0.5f });
	mouseTH->SetTextureSize({
		texSize,
		texSize / 2 });
	mouseTH->SetTextureLeftTop({
		texSize,
		(texSize * 2) + (texSize / 2) });
	//mouseSpriteTextH_.reset(newSprite);
	mouseSprites_.push_back(std::move(mouseTH));
}
void GamePlayScene::InitializeBlackOut() {
	//暗幕
	Fade* newFade = new Fade();
	blackOut_.reset(newFade);
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

	ArrangeTile* newArrangeTile_ = new ArrangeTile();
	arrangeTile_.reset(newArrangeTile_);
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

void GamePlayScene::UpdateCamera() {
	if (player_->IsStart()) {
		followCamera_->Update();
	}
	else {
		Vector3 startCameraMove = followCamera_->GetWorldTransform()->position_;
		startCameraMove += {0.0f, 0.0f, -0.2f};
		followCamera_->GetWorldTransform()->position_ = startCameraMove;
	}

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
void GamePlayScene::UpdateBackground() {
	skydome_->Update();

	for (std::unique_ptr<Object3d>& obj : objs_) {
		obj->Update();
	}
#pragma region Tube
	tubeManager_->Update();
#pragma endregion
}
void GamePlayScene::UpdateCharacter() {
	if (player_->IsStart()) {
		//敵機の更新
		for (std::unique_ptr<Enemy>& enemy : enemys_) {
			enemy->Update();
			{
			//if (!cursor_.IsLockOn()) {
			//	enemyWorldPos_ = {
			//		enemy->GetMatWorld().m[3][0],
			//		enemy->GetMatWorld().m[3][1],
			//		enemy->GetMatWorld().m[3][2]
			//	};

			//}

			//if (!player_->IsDead()) {
			//	//自機と敵機の距離(仮)
			//	float distancePToE =
			//		Vector3Dot(
			//			Vector3{
			//			enemy->GetMatWorld().m[3][0],
			//			enemy->GetMatWorld().m[3][1],
			//			enemy->GetMatWorld().m[3][2] },

			//			Vector3{
			//			player_->GetMatWorld().m[3][0],
			//			player_->GetMatWorld().m[3][1],
			//			player_->GetMatWorld().m[3][2] });

			//	//カーソルから3Dレティクルまでの距離を設定
			//	cursor_.SetDistance(distancePToE);

			//	//マウスカーソルから、3D照準座標を取得する
			//	LockOnTargetPos_ =
			//		cursor_.Get3DReticlePosition(camera_.get(), enemyWorldPos_);;
			//}
			}


		}
	}

	//自機のレティクル更新
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		player_->UpdateReticle(enemy.get());
	}
	player_->Update();
}
void GamePlayScene::UpdateDamage() {
	if (player_->IsStart()) {
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
}
void GamePlayScene::UpdateMouseUI() {
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
void GamePlayScene::UpdateBlackOut() {
	blackOut_->Update();

	if (arrangeTile_->IsOpen()) {
		arrangeTile_->Update();
	}

	if (arrangeTile_->IsEnd()) {
		arrangeTile_->SetIs(false);
	}
}

void GamePlayScene::DrawParticle() {
	pm_->Draw();
}
void GamePlayScene::DrawMouseUI() {
	for (std::unique_ptr<Sprite>& mouse : mouseSprites_) {
		mouse->Draw();
	}
}
void GamePlayScene::DrawCharacterUI() {
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->DrawUI();
	}
	player_->DrawUI();
}
void GamePlayScene::DrawBlackOut() {
	blackOut_->Draw();
	arrangeTile_->Draw();
}
void GamePlayScene::DrawCharacter() {
	//敵機描画
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Draw();
	}

	//自機描画
	player_->Draw();
}
void GamePlayScene::DrawBackground() {
	//天球描画
	skydome_->Draw();
	for (std::unique_ptr<Object3d>& obj : objs_) {
		obj->Draw();
	}
#pragma region Tube
	tubeManager_->Draw();
#pragma endregion
}

void GamePlayScene::RemoveUniquePtr() {
	//敵機をデスフラグで削除
	enemys_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return enemy->IsDead();
		});
}
void GamePlayScene::DebugShortCut() {
	if (player_->IsStart()) {
#ifdef _DEBUG
		if (input_->TriggerKey(DIK_K)) {
			//敵機のダメージ処理
			for (std::unique_ptr<Enemy>& enemy : enemys_) {
				enemy->SetIsDead(true);
			}
		}
#endif // _DEBUG
	}

#ifdef _DEBUG
	if (input_->ReleaseKey(DIK_Q)) {
		isDebugCamera_ = !isDebugCamera_;
	}
#endif // _DEBUG
}
void GamePlayScene::PhaseChange() {
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
		else {
			if (!arrangeTile_->IsOpen()) {
				arrangeTile_->Update();
			}
			else {
				arrangeTile_->Reset(true, false);
			}

			if (arrangeTile_->IsEnd()) {
				//シーンの切り替えを依頼
				SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
			}
		}
	}
}

void GamePlayScene::AddEnemy(
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
	newEnemy->SetGamePlayScene(this);
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

void GamePlayScene::Initialize3d() {
	InitializeCamera();
	InitializeCharacter();
	InitializeBackground();

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	Object3d::SetLight(light_);

	//パーティクル
	Particle* newParticle = Particle::LoadFromObjModel("particle.png");
	particle_.reset(newParticle);

	ParticleManager* newPM = ParticleManager::Create();
	pm_.reset(newPM);
	pm_->SetCamera(camera_.get());
	pm_->SetParticleModel(particle_.get());
	pm_->SetColor({ 0.7f,0.4f,0.1f,0.7f });
}
void GamePlayScene::Initialize2d() {
	InitializeMouseUI();
	InitializeBlackOut();
}

void GamePlayScene::Update3d() {
	{
		static Vector3 lightDir = { 0,1,5 };
		light_->SetDirLightDir(0, lightDir);
	}
	RemoveUniquePtr();
	if (player_->IsStart()) {
		DebugShortCut();

		PhaseChange();

		UpdateCamera();

		UpdateDamage();

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

	UpdateCharacter();
	UpdateBackground();
	UpdateCamera();

	light_->Update();

	pm_->Update();
}
void GamePlayScene::Update2d() {
	UpdateMouseUI();
	UpdateBlackOut();
}

void GamePlayScene::Draw3d() {
	DrawBackground();
	DrawCharacter();
}
void GamePlayScene::Draw2d() {
	DrawMouseUI();
	DrawCharacterUI();
	DrawBlackOut();
}

void GamePlayScene::Initialize() {
	Initialize3d();
	Initialize2d();
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
