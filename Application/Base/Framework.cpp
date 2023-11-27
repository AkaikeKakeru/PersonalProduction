/*コードの全体の処理を、
初期化　更新　描画のグループとして三つに独立させる*/

/*プロジェクト内の、ウィンドウ作成や基盤の初期化等の枠組みを行う*/

#include "Framework.h"
#include "Object3d.h"
#include "SpriteBasis.h"
#include "ParticleManager.h"
#include "TitleScene.h"
#include <imgui.h>
#include <AdjustmentVariables.h>

SceneManager* Framework::sceneManager_ = SceneManager::GetInstance();

void Framework::Run(){
	/// 初期化
	Initialize();

	/// ゲームループ
	while (true) {
		/// 更新
		Update();

		if (IsEndRequest()) {
			//終了リクエストが来たら抜ける
			break;
		}

		/// 描画
		Draw();
	}

	Finalize();
}

void Framework::Initialize(){
	///基盤初期化
	//アプリケーション
	winApp_ = WinApp::GetInstance();
	winApp_->Initialize();

	//DirectX基盤
	dxBas_ = DirectXBasis::GetInstance();
	dxBas_->Initialize(winApp_);

	//Input
	input_ = Input::GetInstance();
	input_->Initialize();

	//音声
	audio_ = Audio::GetInstance();
	audio_->Initialize();
#ifdef _DEBUG
	//ImGuiマネージャー
	ImGuiManager::GetInstance();
	ImGuiManager::StaticInitialize();
#endif // DEBUG

	//オブジェクト基盤
	Object3d::StaticInitialize(dxBas_->GetDevice().Get());

	//描画基盤(スプライト)
	SpriteBasis* spriteBas = SpriteBasis::GetInstance();
	spriteBas->Initialize();
	spriteBas->LoadTexture(kSampleTextureIndex_, "texture.png");
	spriteBas->LoadTexture(kCursorTextureIndex_, "cursor.png");
	spriteBas->LoadTexture(kGaugeTextureIndex_, "HPgauge.png");
	spriteBas->LoadTexture(kBackgroundTextureIndex_, "remains_BG.png");

	spriteBas->LoadTexture(kWhiteTextureIndex_, "white.png");
	spriteBas->LoadTexture(kTextTextureIndex_, "debugfont.png");

	//パーティクルマネージャー
	ParticleManager::StaticInitialize(dxBas_->GetDevice().Get());

	//ライト静的初期化
	LightGroup::StaticInitialize(dxBas_->GetDevice().Get());

	//ポストエフェクトの初期化
	postEffect_ = new PostEffect();
	postEffect_->Initialize();

	//調整項目の読込
	AdjustmentVariables::GetInstance()->LoadFiles();
}

void Framework::Update(){
	//windowsのメッセージ処理
	if (winApp_->ProcessMessage()) {
		//ゲームループを抜ける
		isEndRequest_ = true;
	}

	sceneManager_->Update();
}

void Framework::Finalize(){
	audio_->Finalize();
	Audio::GetInstance()-> SoundUnload(&soundData1);
#ifdef _DEBUG
	imGuiManager_->Finalize();
#endif // DEBUG
	sceneManager_->Finalize();

	delete postEffect_;
	delete sceneFactory_;
}
