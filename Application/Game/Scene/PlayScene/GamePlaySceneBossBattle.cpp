#include "GamePlaySceneBossBattle.h"

#include "GamePlayScene.h"

#pragma region popLoader
#include <fstream>
#include <Framework.h>
#pragma endregion

#include <cassert>
#include <Quaternion.h>
#include <Random.h>

#include "SceneManager.h"

Input* GamePlaySceneBossBattle::input_ = Input::GetInstance();
ObjectManager* GamePlaySceneBossBattle::objManager_ = ObjectManager::GetInstance();
Camera* GamePlaySceneBossBattle::camera_ = GamePlayScene::GetCamera();
void GamePlaySceneBossBattle::InitializeCamera() {
	//カメラ生成
	camera_ = GamePlayScene::GetCamera();
	camera_->SetEye({ 0.0f,0.0f,50.0f });
	camera_->SetTarget({ 0.0f,0.0f,0.0f });
	camera_->SetUp({ 0.0f,1.0f,0.0f });
	camera_->Update();

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(
		{ 30,30,-200 },
		{ ConvertToRadian(10),0,0 });

	//追従カメラの生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	camera_->SetEye({ 0.0f,0.0f,50.0f });
	camera_->SetTarget({ 0.0f,0.0f,0.0f });
	camera_->SetUp({ 0.0f,1.0f,0.0f });
}
void GamePlaySceneBossBattle::InitializeCharacter() {
	Player* newPlayer = Player::Create(
		objManager_->GetModel(playerActiveModel_));
	newPlayer->SetBulletModel(
		objManager_->GetModel(bulletModel_));
	newPlayer->SetModelPauses(
		objManager_->GetModel(playerActiveModel_),
		objManager_->GetModel(playerHideModel_));
	newPlayer->SetCartModel(objManager_->GetModel(cartModel_));
	newPlayer->SetCamera(camera_);
	newPlayer->Initialize();

	Ease ease;
	ease.Reset(
		Ease::In_,
		50,{
			0.0f,
			newPlayer->kDefaultPosY_,
			0.0f
		},{
			0.0f,
			newPlayer->kDefaultPosY_,
			newPlayer->kDefaultPosZ_
		});
	newPlayer->SetStartPositionEase(ease);

	ease.Reset(
		Ease::In_,
		50,
		newPlayer->GetRotation(),{
			newPlayer->GetRotation().x,
			newPlayer->GetRotation().y + ConvertToRadian(180.0f),
			newPlayer->GetRotation().z
		});
	newPlayer->SetStartRotationEase(ease);
	player_.reset(newPlayer);

	const int bossPhaseIndex = 100;

	player_->SetPhaseAdvance(true, bossPhaseIndex);
	player_->Update();

	//追従カメラの更新
	followCamera_->SetTargetWorldTransform(player_->GetWorldTransform());
	followCamera_->SetEye({
		player_->GetPosition().x,
		player_->GetPosition().y,
		player_->GetPosition().z - 30.0f }
	);
	followCamera_->Update();
#pragma endregion

	Boss* newBoss = Boss::Create(objManager_->GetModel(enemyModel_));
	newBoss->SetCamera(camera_);
	newBoss->SetPosition({ 0,20,150 });
	newBoss->SetRotation({ 0,0,0 });
	newBoss->SetScale({ 50,50,50 });
	newBoss->SetBulletModel(
		objManager_->GetModel(bulletModel_));
	newBoss->SetIsStart(true);
	newBoss->SetTargetPos(
		player_->GetPosWorld());

	newBoss->SetLife(200.0f);
	newBoss->Initialize();

	ease.Reset(
		Ease::In_,
		50,{
			0.0f,
			newBoss->kDefaultPosY_,
			0.0f
		},{
			newBoss->kDefaultPosX_,
			newBoss->kDefaultPosY_,
			newBoss->kDefaultPosZ_
		});
	newBoss->SetStartPositionEase(ease);

	boss_.reset(newBoss);
	boss_->Update();
}

void GamePlaySceneBossBattle::UpdateCamera() {
	if (player_->IsStart()) {

	}
	else {
		Vector3 startCameraMove = followCamera_->GetWorldTransform()->position_;
		startCameraMove += {0.0f, 0.0f, -0.2f};
		followCamera_->GetWorldTransform()->position_ = startCameraMove;
	}

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
void GamePlaySceneBossBattle::UpdateCharacter() {
	Vector2 cursorPos2d{}; 
	if (player_->IsStart()) {

		Vector3 playerWorldPos =
			player_->GetPosWorld();

		boss_->SetTargetPos(playerWorldPos);

		//敵機の更新
		boss_->Update();


		//for (std::unique_ptr<WeakPoint>& weak : w) {
		//	if (weak) {
		//		weak->SetPosition(Object3d::GetPosition());
		//		weak->Update();
		//	}
		//}

		//float distanceCameraToEnemy =
		//	boss_->GetPosWorld().z - camera_->GetEye().z;


		//for (int i = 0; i < boss_->kWeakPointCount_; i++) {
		//	Cursor c;
		//	Vector3 curPosEx =
		//	boss_->GetWeakPos()[i];

		//	c.SetDistance(distanceCameraToEnemy);

		//	c.Get3DReticlePosition(camera_, curPosEx);
		//	if(c.IsLockOn()) {
		//		
		//		enemyWorldPos_ = curPosEx;
		//	}
		//}

		////自機のレティクル更新
		//player_->
		//	UpdateReticle(enemyWorldPos_, distanceCameraToEnemy);//,cursorPos2d);
		//	//UpdateReticle(curPosEx, distanceCameraToEnemy);//,cursorPos2d);
	}
	player_->Update();
}
void GamePlaySceneBossBattle::UpdateDamage() {
	if (player_->IsStart()) {
		//敵機のダメージ処理
		boss_->Update();
		//被ダメージ処理
		if (boss_->IsDamage()) {
			float life = boss_->GetLife();

			life -= nowDamagePlayer_;
			boss_->SetLife(life);

			boss_->SetIsDamage(false);
		}
	}
}

void GamePlaySceneBossBattle::DrawCharacter() {
	//敵機描画
	boss_->Draw();

	//自機描画
	player_->Draw();
}
void GamePlaySceneBossBattle::DrawParticle() {
}
void GamePlaySceneBossBattle::DrawCharacterUI() {
	boss_->DrawUI();
	player_->DrawUI();
}

void GamePlaySceneBossBattle::FinalizeCharacter() {
}

void GamePlaySceneBossBattle::RemoveUniquePtr() {
}
void GamePlaySceneBossBattle::DebugShortCut() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_K)) {
		//敵機のダメージ処理
		boss_->SetIsDead(true);
	}
#endif // _DEBUG

#ifdef _DEBUG
	if (input_->ReleaseKey(DIK_Q)) {
		isDebugCamera_ = !isDebugCamera_;
	}
#endif // _DEBUG
}

void GamePlaySceneBossBattle::PhaseChange() {
	//敵機が全滅したら(コンテナが空になったら)
	if (boss_->IsDead()) {
		//シーンの切り替えを依頼
		SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
	}
}

void GamePlaySceneBossBattle::Initialize3d() {
}

void GamePlaySceneBossBattle::InitializeParticle() {
}

void GamePlaySceneBossBattle::Initialize() {
	cursor_.SetDistance(0.0f);
	InitializeCamera();
	InitializeCharacter();
	InitializeParticle();
}
void GamePlaySceneBossBattle::Update() {
	Update3d();
	Update2d();
}
void GamePlaySceneBossBattle::Draw() {
}
void GamePlaySceneBossBattle::Finalize() {
	followCamera_->Finalize();
	boss_->Finalize();
	player_->Finalize();
}

void GamePlaySceneBossBattle::Update3d() {
	RemoveUniquePtr();
	DebugShortCut();

	PhaseChange();

	UpdateCamera();

	UpdateDamage();

	UpdateCharacter();
	UpdateCamera();
}

void GamePlaySceneBossBattle::Update2d() {
}

void GamePlaySceneBossBattle::Draw3d() {
	//敵機描画
	boss_->Draw();

	//自機描画
	player_->Draw();
}
void GamePlaySceneBossBattle::Draw2d() {
	boss_->DrawUI();
	player_->DrawUI();
}

void GamePlaySceneBossBattle::DrawImGui() {
	//boss_->DrawImgui();
	//player_->DrawImgui();
}
