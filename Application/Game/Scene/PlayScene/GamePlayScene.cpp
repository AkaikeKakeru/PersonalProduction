/*ゲームプレイシーン*/

#include "GamePlayScene.h"
#include "SafeDelete.h"
#include "Quaternion.h"
#include "CollisionManager.h"
#include "Random.h"
#include "Cursor.h"

#pragma region popLoader
#include <fstream>
#include <Framework.h>
#pragma endregion

#include "LevelLoader.h"

#include "SceneManager.h"
#include "PlaySceneStateFactory.h"


#include "GameMain.h"

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

PlaySceneStateManager* GamePlayScene::stateManager_ = PlaySceneStateManager::GetInstance();

Camera* GamePlayScene::camera_ = GamePlayScene::GetCamera();

void GamePlayScene::Finalize() {
	SafeDelete(light_);
	SafeDelete(camera_);
	SafeDelete(buttonPause_);

	stateManager_->Finalize();

	FinalizeBlackOut();
	FinalizeBackGround();
	
	delete stateFactory_;
}

void GamePlayScene::FinalizeBackGround() {
#pragma region Tube
	//tubeManager_->Finalize();
#pragma endregion

	stageField_->Finalize();

	skydome_->Finalize();
}
void GamePlayScene::FinalizeCharacter() {
}
void GamePlayScene::FinalizeBlackOut() {
	blackOut_->Finalize();
	arrangeTile_->Finalize();
}

void GamePlayScene::InitializeCamera() {
	//カメラ生成
	camera_ = new Camera();

	camera_->SetEye({ 0.0f,10.0f,-20.0f });
	camera_->SetTarget({ 0.0f,0.0f,0.0f });
	camera_->SetUp({ 0.0f,1.0f,0.0f });
	camera_->Update();
}
void GamePlayScene::InitializeBackground() {
#pragma region Skydome
	Skydome* newSkydome = Skydome::Create();
	skydome_.reset(newSkydome);
	skydome_->SetCamera(camera_);
	skydome_->SetModel(objManager_->GetModel(skydomeModel_));
	skydome_->SetScale({ 1024.0f, 256.0f, 1024.0f });
	skydome_->SetPosition({ 0,0,0 });
#pragma endregion

	stageField_ = std::make_unique<StageField>();
	stageField_->SetJsonFileName("StageField1");
	stageField_->SetCamera(*camera_);
	stageField_->Initialize();

//#pragma region Tube
//	TubeManager* newTubeManager_ = new TubeManager();
//	tubeManager_.reset(newTubeManager_);
//	tubeManager_->SetCamera(camera_);
//	tubeManager_->SetSpeed(16.0f);
//	tubeManager_->SetRotation(CreateRotationVector(
//		{ 0.0f,0.0f,1.0f }, ConvertToRadian(180.0f)));
//	tubeManager_->SetScale({ 100,100,100 });
//	tubeManager_->SetTubeModel(objManager_->GetModel(tubeModel_));
//	tubeManager_->Initialize();
//#pragma endregion
//
//#pragma region 扉
//	Object3d* obj = new Object3d();
//
//	std::unique_ptr<Object3d> newObj =
//		std::make_unique<Object3d>();
//
//	doorPos_ = { 0,0,800 };
//
//	obj = Object3d::Create();
//	obj->SetCamera(camera_);
//	obj->SetScale({ 50,400,10 });
//	Vector3 scaDoorL = obj->GetScale();
//
//	obj->SetPosition({ -scaDoorL.x, scaDoorL.y / 2,doorPos_.z });
//	obj->SetRotation(CreateRotationVector(
//		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
//	obj->SetModel(objManager_->GetModel(doorModel_));
//
//	newObj.reset(obj);
//	objs_.push_back(std::move(newObj));
//
//	obj = Object3d::Create();
//	obj->SetCamera(camera_);
//	obj->SetScale({ 50,400,10 });
//	Vector3 scaDoorR = obj->GetScale();
//	obj->SetPosition({ scaDoorR.x, scaDoorR.y / 2, doorPos_.z });
//	obj->SetRotation(CreateRotationVector(
//		{ 0.0f,1.0f,0.0f }, ConvertToRadian(0.0f)));
//	obj->SetModel(objManager_->GetModel(doorModel_));
//	newObj.reset(obj);
//	objs_.push_back(std::move(newObj));
//#pragma endregion
//
//	obj = Object3d::Create();
//	obj->SetModel(objManager_->GetModel(bottomBGModel_));
//	obj->SetScale({ 10.0f, 10.0f, 10.0f });
//	obj->SetPosition({ 0,-100,0 });
//	obj->SetCamera(camera_);
//	obj->Update();
//
//	newObj.reset(obj);
//	objs_.push_back(std::move(newObj));
}
void GamePlayScene::InitializeCharacter() {
}

void GamePlayScene::InitializeMouseUI() {
	float size = 64.0f;
	float texSize = 64.0f;
	float kCenterAnchorPoint_ = 0.5f;

	std::unique_ptr<Sprite> mouseL =
		std::make_unique<Sprite>();
	mouseL->Initialize(Framework::kMouseTextureIndex_);
	mouseL->SetPosition({
		WinApp::Win_Width - (texSize * 2),
		WinApp::Win_Height - (texSize * 2) });
	mouseL->SetSize({ size, size * 2 });
	mouseL->SetAnchorPoint({
		kCenterAnchorPoint_,
		kCenterAnchorPoint_ });
	mouseL->SetTextureSize({
		texSize,
		(texSize * 2) });
	mouseL->SetTextureLeftTop({
		0,
		0 });
	mouseSprites_.push_back(std::move(mouseL));

	std::unique_ptr<Sprite> mouseR =
		std::make_unique<Sprite>();
	mouseR->Initialize(Framework::kMouseTextureIndex_);
	mouseR->SetPosition({
		WinApp::Win_Width - texSize,
		WinApp::Win_Height - (texSize * 2) });
	mouseR->SetSize({ size, size * 2 });
	mouseR->SetAnchorPoint({
		kCenterAnchorPoint_,
		kCenterAnchorPoint_ });
	mouseR->SetTextureSize({
		texSize,
		(texSize * 2) });
	mouseR->SetTextureLeftTop({
		texSize,
		0 });
	mouseSprites_.push_back(std::move(mouseR));

	std::unique_ptr<Sprite> mouseW =
		std::make_unique<Sprite>();
	mouseW->Initialize(Framework::kMouseTextureIndex_);
	mouseW->SetPosition({
		WinApp::Win_Width - (texSize * 1.5f),
		WinApp::Win_Height - (texSize * 2) });
	mouseW->SetSize({ size, size });
	mouseW->SetAnchorPoint({
		kCenterAnchorPoint_,
		kCenterAnchorPoint_ });
	mouseW->SetTextureSize({
		texSize,
		texSize });
	mouseW->SetTextureLeftTop({
		0,
		(texSize * 2) });
	mouseSprites_.push_back(std::move(mouseW));

	std::unique_ptr<Sprite> mouseTS =
		std::make_unique<Sprite>();
	mouseTS->Initialize(Framework::kMouseTextureIndex_);
	mouseTS->SetPosition({
		WinApp::Win_Width - (texSize * 2),
		WinApp::Win_Height - (texSize * 1.5f) });
	mouseTS->SetSize({ size, size / 2 });
	mouseTS->SetAnchorPoint({
		kCenterAnchorPoint_,
		kCenterAnchorPoint_ });
	mouseTS->SetTextureSize({
		texSize,
		texSize / 2 });
	mouseTS->SetTextureLeftTop({
		texSize,
		(texSize * 2) });
	mouseSprites_.push_back(std::move(mouseTS));

	std::unique_ptr<Sprite> mouseTH =
		std::make_unique<Sprite>();
	mouseTH->Initialize(Framework::kMouseTextureIndex_);
	mouseTH->SetPosition({
		WinApp::Win_Width - texSize,
		WinApp::Win_Height - (texSize * 1.5f) });
	mouseTH->SetSize({ size, size / 2 });
	mouseTH->SetAnchorPoint({
		kCenterAnchorPoint_,
		kCenterAnchorPoint_ });
	mouseTH->SetTextureSize({
		texSize,
		texSize / 2 });
	mouseTH->SetTextureLeftTop({
		texSize,
		(texSize * 2) + (texSize / 2) });
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

void GamePlayScene::InitializePause() {
	pauseScreen_ = std::make_unique<PauseScreen>();
	pauseScreen_->Initialize();

	float size = 64.0f;
	float texSize = 64.0f;
	float kCenterAnchorPoint_ = 0.5f;
	float textSize = 2.5f;
	//ボタン
	buttonPause_ = new Button();
	buttonPause_->Initialize(0);
	buttonPause_->SetTelop("pause");
	buttonPause_->SetPosition({
		WinApp::Win_Width - (texSize * 3.0f) ,texSize * 2.0f });
	buttonPause_->SetSize({ 200.0f,48.0f });
	buttonPause_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	buttonPause_->GetText()->
		SetSize({ textSize,textSize });
	buttonPause_->GetText()->SetSize({ 0.0f,0.0f });

	std::unique_ptr<Sprite> mouseTP =
		std::make_unique<Sprite>();
	mouseTP->Initialize(Framework::kMouseTextureIndex_);
	mouseTP->SetPosition(buttonPause_->GetPosition());
	mouseTP->SetSize({ size * 1.5f, size / 2 });
	mouseTP->SetAnchorPoint({
		kCenterAnchorPoint_,
		kCenterAnchorPoint_ });
	mouseTP->SetTextureSize({
		texSize * 1.5f,
		texSize / 2 });
	mouseTP->SetTextureLeftTop({
		texSize,
		(texSize * 2) + (texSize / 2) + (texSize / 2)  });
	mouseSprites_.push_back(std::move(mouseTP));
}

void GamePlayScene::UpdateCamera() {
}
void GamePlayScene::UpdateBackground() {
	skydome_->Update();

	for (std::unique_ptr<Object3d>& obj : objs_) {
		obj->Update();
	}
#pragma region Tube
	//tubeManager_->Update();
#pragma endregion
}
void GamePlayScene::UpdateCharacter() {
}
void GamePlayScene::UpdateDamage() {
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
	mouseSprites_[mouseSpriteTextS_]->SetColor(colL);
	
	mouseSprites_[mouseSpriteR_]->SetColor(colR);
	mouseSprites_[mouseSpriteTextH_]->SetColor(colR);
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

void GamePlayScene::UpdatePause() {
	buttonPause_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	if (buttonPause_->ChackClick(input_->ReleaseMouse(0))) {
		if (pauseScreen_->IsRun()) {
			pauseScreen_->ResetEase_Remove();
			pauseScreen_->SetIsRemove(true);
		}
		else {
			pauseScreen_->ResetEase_Run();
		}
	}

	isPause_ = pauseScreen_->IsRun();
}

void GamePlayScene::DrawParticle() {
}
void GamePlayScene::DrawMouseUI() {
	for (std::unique_ptr<Sprite>& mouse : mouseSprites_) {
		mouse->Draw();
	}
}
void GamePlayScene::DrawCharacterUI() {
}
void GamePlayScene::DrawBlackOut() {
	blackOut_->Draw();
	arrangeTile_->Draw();
}
void GamePlayScene::DrawPause(){
	pauseScreen_->Draw();
	buttonPause_->Draw();
}
void GamePlayScene::DrawCharacter() {
}
void GamePlayScene::DrawBackground() {
	//天球描画
	skydome_->Draw();
	for (std::unique_ptr<Object3d>& obj : objs_) {
		obj->Draw();
	}
#pragma region Tube
	//tubeManager_->Draw();
#pragma endregion
}

void GamePlayScene::Initialize3d() {
	InitializeCamera();
	InitializeBackground();

	//ライト生成
	light_ = new LightGroup();
	light_ = LightGroup::Create();
	light_->SetAmbientColor({ 1,1,1 });
	Object3d::SetLight(light_);
}
void GamePlayScene::Initialize2d() {
	InitializeMouseUI();
	InitializeBlackOut();
	InitializePause();
}

void GamePlayScene::Update3d() {
	{
		static Vector3 lightDir = { 0,1,5 };
		light_->SetDirLightDir(0, lightDir);
	}
	UpdateBackground();
	light_->Update();
}
void GamePlayScene::Update2d() {
	UpdateMouseUI();
	UpdateBlackOut();
}

void GamePlayScene::Draw3d() {
	DrawBackground();
}
void GamePlayScene::Draw2d() {
	DrawPause();
	DrawMouseUI();
	DrawBlackOut();
}

void GamePlayScene::Initialize() {
	Initialize3d();

	stateManager_ = GameMain::GetStateManager();

	//シーンファクトリーを生成し、マネージャーにセット
	stateFactory_ = new PlaySceneStateFactory();
	PlaySceneStateManager::GetInstance()->SetSceneFactory(stateFactory_);

	//シーンマネージャーに最初のシーンをセット
	PlaySceneStateManager::GetInstance()->ChangeState("BOSSBATTLE");

	Initialize2d();
}
void GamePlayScene::Update() {
	input_->Update();

	UpdatePause();

	if (isPause_) {
		pauseScreen_->Update();
	}
	else {
		stateManager_->Update();

		Update3d();
		Update2d();

		if (stateManager_) {
			collisionManager_->CheckAllCollisions();
		}
	}

	buttonPause_->Update();
}
void GamePlayScene::Draw() {
#ifdef _DEBUG
	imGuiManager_->Begin();

	stateManager_->DrawImGui();

	imGuiManager_->End();
#endif // DEBUG
	Object3d::PreDraw(dxBas_->GetCommandList().Get());
	stateManager_->Draw3d();
	Draw3d();
	Object3d::PostDraw();

	ParticleManager::PreDraw(dxBas_->GetCommandList().Get());
	stateManager_->DrawParticle();
	DrawParticle();
	ParticleManager::PostDraw();

	SpriteBasis::GetInstance()->PreDraw();
	stateManager_->Draw2d();
	Draw2d();
	SpriteBasis::GetInstance()->PostDraw();
}
